#include "SLDM_Generator.h"

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/SpotLight.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Camera.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace Define;
using namespace std;

const string rootPath = ROOT_PATH;

namespace CppUtil {
	namespace Engine {
		const string str_PointLight_prefix = "data/shaders/Engine/PointLight/";
		const string str_genDepth = "genDepth";
		const string str_genDepth_vs = str_PointLight_prefix + str_genDepth + ".vs";
		const string str_genDepth_gs = str_PointLight_prefix + str_genDepth + ".gs";
		const string str_genDepth_fs = str_PointLight_prefix + str_genDepth + ".fs";
	}
}

SLDM_Generator::SLDM_Generator(QT::RawAPI_OGLW * pOGLW, Ptr<Camera> camera, float lightNear, float lightFar)
	: pOGLW(pOGLW), camera(camera), depthMapSize(1024), lightNear(lightNear), lightFar(lightFar)
{
	RegMemberFunc<Scene>(&SLDM_Generator::Visit);
	RegMemberFunc<SObj>(&SLDM_Generator::Visit);

	RegMemberFunc<Sphere>(&SLDM_Generator::Visit);
	RegMemberFunc<Plane>(&SLDM_Generator::Visit);
	RegMemberFunc<TriMesh>(&SLDM_Generator::Visit);
}

void SLDM_Generator::OGL_Init() {
	shader_genDepth = Shader(ROOT_PATH + str_MVP_P3_vs, ROOT_PATH + str_empty_fs);
}

void SLDM_Generator::Visit(Ptr<Scene> scene) {
	if (!scene || !scene->GetRoot() || !camera) {
		printf("ERROR::SLDM_Generator::Visit(Ptr<Scene> scene):\n"
			"\t""scene or scene's root or camera is nullptr\n");
		return;
	}

	scene->SetWriteLock(true);

	GLint origFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFBO);
	GLint origViewport[4];
	glGetIntegerv(GL_VIEWPORT, origViewport);

	modelVec.clear();
	modelVec.push_back(Transform(1.f));

	// regist
	for (auto cmptLight : scene->GetCmptLights()) {
		if (lightMap.find(cmptLight) != lightMap.end())
			continue;

		auto spotLight = CastTo<SpotLight>(cmptLight->light);
		if (!spotLight)
			continue;

		FBO FBO_DepthMap(depthMapSize, depthMapSize, FBO::ENUM_TYPE_DEPTH);
		Texture depthMap(FBO_DepthMap.GetDepthTexture().GetID(), Texture::ENUM_TYPE_2D);
		lightMap[cmptLight] = FBO_Tex(FBO_DepthMap, depthMap);
	}

	light2pv.clear();
	auto nextIt = lightMap.begin();
	while (nextIt != lightMap.end()) {
		auto curIt = nextIt;
		nextIt++;

		if (curIt->first.expired()) {
			lightMap.erase(curIt);
			continue;
		}
		auto lightComponent = curIt->first.lock();
		auto spotLight = CastTo<SpotLight>(lightComponent->light);

		curIt->second.fbo.Use();
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, depthMapSize, depthMapSize);
		glClear(GL_DEPTH_BUFFER_BIT);

		auto l2w = lightComponent->GetSObj()->GetLocalToWorldMatrix();
		auto pos = l2w(Point3(0.f));
		auto dir = l2w(Normalf(0, -1, 0));

		auto view = Transform::LookAt(pos, pos + dir);

		auto proj = Transform::Perspcetive(spotLight->angle, 1.f, lightNear, lightFar);

		shader_genDepth.SetMat4f("view", view.GetMatrix().Data());
		shader_genDepth.SetMat4f("proj", proj.GetMatrix().Data());

		light2pv[lightComponent] = proj * view;

		scene->GetRoot()->Accept(This());
	}

	glBindFramebuffer(GL_FRAMEBUFFER, origFBO);
	glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);

	scene->SetWriteLock(false);
}

void SLDM_Generator::Visit(Ptr<SObj> sobj) {
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto children = sobj->GetChildren();
	// 这种情况下不需要 transform
	if ((geometry == nullptr || !geometry->primitive) && children.size() == 0)
		return;

	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform != nullptr)
		modelVec.push_back(modelVec.back() * cmptTransform->GetTransform());

	if (geometry && geometry->primitive)
		geometry->primitive->Accept(This());

	for (auto child : children)
		child->Accept(This());

	if (cmptTransform != nullptr)
		modelVec.pop_back();
}


void SLDM_Generator::Visit(Ptr<Sphere> sphere) {
	shader_genDepth.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Sphere).Draw(shader_genDepth);
}

void SLDM_Generator::Visit(Ptr<Plane> plane) {
	shader_genDepth.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Plane).Draw(shader_genDepth);
}

void SLDM_Generator::Visit(Ptr<TriMesh> mesh) {
	shader_genDepth.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(mesh).Draw(shader_genDepth);
}

const Texture SLDM_Generator::GetDepthMap(PtrC<CmptLight> light) const {
	auto target = lightMap.find(light);
	if (target == lightMap.end())
		return Texture::InValid;

	return target->second.tex;
}

const Transform SLDM_Generator::GetProjView(PtrC<CmptLight> light) const {
	auto target = light2pv.find(light);
	if (target == light2pv.cend())
		return Transform();

	return target->second;
}
