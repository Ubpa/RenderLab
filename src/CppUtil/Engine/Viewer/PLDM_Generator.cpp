#include "PLDM_Generator.h"

#include <CppUtil/Engine/RasterBase.h>

#include <CppUtil/Qt/RawAPI_Define.h>


#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/PointLight.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

namespace CppUtil {
	namespace Engine {
		const string str_PointLight_prefix = "data/shaders/Engine/PointLight/";
		const string str_genDepth = "genDepth";
		const string str_genDepth_vs = str_PointLight_prefix + str_genDepth + ".vs";
		const string str_genDepth_gs = str_PointLight_prefix + str_genDepth + ".gs";
		const string str_genDepth_fs = str_PointLight_prefix + str_genDepth + ".fs";
	}
}

PLDM_Generator::PLDM_Generator(RasterBase * raster)
	: raster(raster), depthMapSize(1024), lightFar(25.0f)
{
	Reg<Scene>();
	Reg<SObj>();

	Reg<Sphere>();
	Reg<Plane>();
	Reg<TriMesh>();
}

void PLDM_Generator::Init() {
	shader_genDepth = Shader(ROOT_PATH + str_genDepth_vs, ROOT_PATH + str_genDepth_fs, ROOT_PATH + str_genDepth_gs);
	shader_genDepth.UniformBlockBind("Camera", 0);
	shader_genDepth.SetFloat("lightFar", lightFar);
}

void PLDM_Generator::Visit(Scene::Ptr scene) {
	scene->SetWriteLock(true);

	if (!raster || !scene || !scene->GetRoot()) {
		printf("WARN: PLDM_Generator raster or scene or root is nullptr\n");
		return;
	}

	GLint lastFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &lastFBO);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	modelVec.clear();
	modelVec.push_back(Transform(1.f));

	// regist
	for (auto cmptLight : scene->GetCmptLights()) {
		if (lightMap.find(cmptLight) != lightMap.end())
			continue;

		auto pointLight = PointLight::CPtr::Cast(cmptLight->light);
		if (!pointLight)
			continue;

		FBO FBO_DepthMap(depthMapSize, depthMapSize, FBO::ENUM_TYPE_CUBE_DEPTH);
		Texture depthMap(FBO_DepthMap.GetDepthTexture().GetID(), Texture::ENUM_TYPE_CUBE_MAP);
		lightMap[cmptLight] = Val(FBO_DepthMap, depthMap);
	}

	auto shadowProj = Transform::Perspcetive(90.f, 1.f, 0.01f, lightFar);
	auto nextIt = lightMap.begin();
	while (nextIt != lightMap.end()) {
		auto curIt = nextIt;
		nextIt++;

		if (curIt->first.expired()) {
			lightMap.erase(curIt);
			continue;
		}
		auto lightComponent = curIt->first.lock();

		curIt->second.fbo.Use();
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, depthMapSize, depthMapSize);
		glClear(GL_DEPTH_BUFFER_BIT);

		auto lightPos = lightComponent->GetSObj()->GetWorldPos();
		shader_genDepth.SetVec3f("lightPos", lightPos);

		std::vector<Transform> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			Transform::LookAt(lightPos, lightPos + Vec3(1.0, 0.0, 0.0), Vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			Transform::LookAt(lightPos, lightPos + Vec3(-1.0, 0.0, 0.0), Vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			Transform::LookAt(lightPos, lightPos + Vec3(0.0, 1.0, 0.0), Vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			Transform::LookAt(lightPos, lightPos + Vec3(0.0, -1.0, 0.0), Vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			Transform::LookAt(lightPos, lightPos + Vec3(0.0, 0.0, 1.0), Vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			Transform::LookAt(lightPos, lightPos + Vec3(0.0, 0.0, -1.0), Vec3(0.0, -1.0, 0.0)));
		for (size_t i = 0; i < 6; ++i)
			shader_genDepth.SetMat4f("shadowMatrices[" + to_string(i) + "]", shadowTransforms[i].GetMatrix());

		scene->GetRoot()->Accept(This());
	}

	glBindFramebuffer(GL_FRAMEBUFFER, lastFBO);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	scene->SetWriteLock(false);
}

void PLDM_Generator::Visit(Ptr<SObj> sobj) {
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


void PLDM_Generator::Visit(Sphere::Ptr sphere) {
	shader_genDepth.SetMat4f("model", modelVec.back());
	raster->GetSphereVAO().Draw(shader_genDepth);
}

void PLDM_Generator::Visit(Plane::Ptr plane) {
	shader_genDepth.SetMat4f("model", modelVec.back());
	raster->GetPlaneVAO().Draw(shader_genDepth);
}

void PLDM_Generator::Visit(TriMesh::Ptr mesh) {
	shader_genDepth.SetMat4f("model", modelVec.back());
	raster->GetMeshVAO(mesh).Draw(shader_genDepth);
}

Texture PLDM_Generator::GetDepthCubeMap(CmptLight::CPtr light) const {
	auto target = lightMap.find(light);
	if (target == lightMap.end())
		return Texture::InValid;

	return target->second.tex;
}
