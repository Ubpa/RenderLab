#include "DLDM_Generator.h"

#include <Qt/RawAPI_Define.h>
#include <Qt/RawAPI_OGLW.h>

#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SObj.h>

#include <Engine/Scene/CmptTransform.h>

#include <Engine/Scene/CmptLight.h>
#include <Engine/Light/DirectionalLight.h>

#include <Engine/Scene/CmptGeometry.h>
#include <Engine/Primitive/Sphere.h>
#include <Engine/Primitive/Plane.h>
#include <Engine/Primitive/TriMesh.h>
#include <Engine/Primitive/Disk.h>
#include <Engine/Primitive/Capsule.h>

#include <OpenGL/CommonDefine.h>
#include <OpenGL/Camera.h>

#include <ROOT_PATH.h>

using namespace Ubpa;

using namespace Define;
using namespace std;

const string rootPath = ROOT_PATH;

namespace Ubpa {
	const string str_PointLight_prefix = "data/shaders/Engine/PointLight/";
	const string str_genDepth = "genDepth";
	const string str_genDepth_vs = str_PointLight_prefix + str_genDepth + ".vs";
	const string str_genDepth_gs = str_PointLight_prefix + str_genDepth + ".gs";
	const string str_genDepth_fs = str_PointLight_prefix + str_genDepth + ".fs";
}

DLDM_Generator::DLDM_Generator(RawAPI_OGLW * pOGLW, Ptr<Camera> camera)
	: pOGLW(pOGLW), camera(camera), depthMapSize(1024)
{
	Regist<Sphere, Plane, TriMesh, Disk, Capsule>();
}

void DLDM_Generator::Init() {
	shader_genDepth = Shader(ROOT_PATH + str_MVP_P3_vs, ROOT_PATH + str_empty_fs);
}

void DLDM_Generator::Visit(Ptr<Scene> scene) {
	if (!scene || !scene->GetRoot() || !camera) {
		printf("ERROR::DLDM_Generator::Visit(Ptr<Scene> scene):\n"
			"\t""scene or scene's root or camera is nullptr\n");
		return;
	}

	scene->SetWriteLock(true);

	GLint origFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFBO);
	GLint origViewport[4];
	glGetIntegerv(GL_VIEWPORT, origViewport);

	modelVec.clear();
	modelVec.push_back(transformf::eye());

	// regist
	for (auto cmptLight : scene->GetCmptLights()) {
		if (lightMap.find(cmptLight) != lightMap.end())
			continue;

		auto directionalLight = CastTo<DirectionalLight>(cmptLight->light);
		if (!directionalLight)
			continue;

		FBO FBO_DepthMap(depthMapSize, depthMapSize, FBO::ENUM_TYPE_DEPTH);
		Texture depthMap(FBO_DepthMap.GetDepthTexture().GetID(), Texture::ENUM_TYPE_2D);
		lightMap[cmptLight] = FBO_Tex(FBO_DepthMap, depthMap);
	}

	const auto corners = camera->Corners();
	vecf3 sum{ 0.f };
	for (auto p : corners)
		sum += p.cast_to<vecf3>();
	const auto center = (sum/corners.size()).cast_to<pointf3>();

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

		curIt->second.fbo.Use();
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, depthMapSize, depthMapSize);
		glClear(GL_DEPTH_BUFFER_BIT);

		auto nDir = (lightComponent->GetSObj()->GetLocalToWorldMatrix() * vecf3(0, -1, 0)).normalize();
		float minD = FLT_MAX;
		float maxD = -FLT_MAX;
		for (auto corner : corners) {
			auto diff = corner - center;
			auto d = diff.dot(nDir);
			if (d < minD)
				minD = d;
			if (d > maxD)
				maxD = d;
		}
		const float backRatio = 0.05f;
		float extent = maxD - minD;
		auto pos = center + (minD - extent * backRatio ) * nDir;

		auto view = transformf::look_at(pos, center);
		
		float maxX = 0.f;
		float maxY = 0.f;
		for (auto corner : corners) {
			auto cornerInCam = view * corner;
			auto absCornerInCam = cornerInCam.abs();
			if (absCornerInCam[0] > maxX)
				maxX = absCornerInCam[0];
			if (absCornerInCam[1] > maxY)
				maxY = absCornerInCam[1];
		}
		auto proj = transformf::orthographic(2 * maxX, 2 * maxY, 0, extent * (1 + backRatio));

		shader_genDepth.SetMatf4("view", view.data());
		shader_genDepth.SetMatf4("proj", proj.data());

		light2pv[lightComponent] = proj * view;

		Visit(scene->GetRoot());
	}

	glBindFramebuffer(GL_FRAMEBUFFER, origFBO);
	glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);

	scene->SetWriteLock(false);
}

void DLDM_Generator::Visit(Ptr<SObj> sobj) {
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto children = sobj->GetChildren();
	// 这种情况下不需要 transform
	if ((geometry == nullptr || !geometry->primitive) && children.size() == 0)
		return;

	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform != nullptr)
		modelVec.push_back(modelVec.back() * cmptTransform->GetTransform());

	if (geometry && geometry->primitive)
		Visit(geometry->primitive);

	for (auto child : children)
		Visit(child);

	if (cmptTransform != nullptr)
		modelVec.pop_back();
}

void DLDM_Generator::ImplVisit(Ptr<Sphere> sphere) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Sphere).Draw(shader_genDepth);
}

void DLDM_Generator::ImplVisit(Ptr<Plane> plane) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Plane).Draw(shader_genDepth);
}

void DLDM_Generator::ImplVisit(Ptr<TriMesh> mesh) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	pOGLW->GetVAO(mesh).Draw(shader_genDepth);
}

void DLDM_Generator::ImplVisit(Ptr<Disk> disk) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", false);
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Disk).Draw(shader_genDepth);
}

void DLDM_Generator::ImplVisit(Ptr<Capsule> capsule) {
	shader_genDepth.SetMatf4("model", modelVec.back().data());
	shader_genDepth.SetBool("isOffset", true);
	shader_genDepth.SetFloat("offset", capsule->height / 2 - 1);
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Capsule).Draw(shader_genDepth);
}

const Texture DLDM_Generator::GetDepthMap(PtrC<CmptLight> light) const {
	auto target = lightMap.find(light);
	if (target == lightMap.end())
		return Texture::InValid;

	return target->second.tex;
}

const transformf DLDM_Generator::GetProjView(PtrC<CmptLight> light) const {
	auto target = light2pv.find(light);
	if (target == light2pv.cend())
		return transformf();

	return target->second;
}
