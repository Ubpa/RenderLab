#include "SLDM_Generator.h"

#include <Qt/RawAPI_Define.h>
#include <Qt/RawAPI_OGLW.h>

#include <Engine/Scene.h>
#include <Engine/SObj.h>

#include <Engine/CmptTransform.h>

#include <Engine/CmptLight.h>
#include <Engine/SpotLight.h>

#include <Engine/CmptGeometry.h>
#include <Engine/Sphere.h>
#include <Engine/Plane.h>
#include <Engine/TriMesh.h>
#include <Engine/Disk.h>
#include <Engine/Capsule.h>

#include <OpenGL/CommonDefine.h>
#include <OpenGL/Camera.h>

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
	RegMemberFunc<Disk>(&SLDM_Generator::Visit);
	RegMemberFunc<Capsule>(&SLDM_Generator::Visit);
}

void SLDM_Generator::Init() {
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
	modelVec.push_back(Ubpa::transformf::eye());

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
		auto pos = l2w * Ubpa::pointf3(0.f);
		auto dir = l2w * Ubpa::normalf(0, -1, 0);

		auto view = Ubpa::transformf::look_at(pos, pos + dir.cast_to<Ubpa::vecf3>());

		auto proj = Ubpa::transformf::perspective(spotLight->angle, 1.f, lightNear, lightFar);

		shader_genDepth.SetMatf4("view", view.data());
		shader_genDepth.SetMatf4("proj", proj.data());

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
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", false);
	pOGLW->GetVAO(ShapeType::Sphere).Draw(shader_genDepth);
}

void SLDM_Generator::Visit(Ptr<Plane> plane) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", false);
	pOGLW->GetVAO(ShapeType::Plane).Draw(shader_genDepth);
}

void SLDM_Generator::Visit(Ptr<TriMesh> mesh) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", false);
	pOGLW->GetVAO(mesh).Draw(shader_genDepth);
}

void SLDM_Generator::Visit(Ptr<Disk> disk) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", false);
	pOGLW->GetVAO(ShapeType::Disk).Draw(shader_genDepth);
}

void SLDM_Generator::Visit(Ptr<Capsule> capsule) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", true);
	shader_genDepth.SetFloat("offset", capsule->height / 2 - 1);
	pOGLW->GetVAO(ShapeType::Capsule).Draw(shader_genDepth);
}

const Texture SLDM_Generator::GetDepthMap(PtrC<CmptLight> light) const {
	auto target = lightMap.find(light);
	if (target == lightMap.end())
		return Texture::InValid;

	return target->second.tex;
}

const Ubpa::transformf SLDM_Generator::GetProjView(PtrC<CmptLight> light) const {
	auto target = light2pv.find(light);
	if (target == light2pv.cend())
		return Ubpa::transformf();

	return target->second;
}
