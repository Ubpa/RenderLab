#include <Engine/Viewer/Raster.h>

#include "DLDM_Generator.h"
#include "PLDM_Generator.h"
#include "SLDM_Generator.h"
#include "EnvGenerator.h"

#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SObj.h>

#include <Engine/Scene/CmptLight.h>
#include <Engine/Light/PointLight.h>
#include <Engine/Light/DirectionalLight.h>
#include <Engine/Light/SpotLight.h>
#include <Engine/Light/InfiniteAreaLight.h>
#include <Engine/Light/SphereLight.h>
#include <Engine/Light/DiskLight.h>
#include <Engine/Light/AreaLight.h>
#include <Engine/Light/CapsuleLight.h>

#include <Qt/RawAPI_OGLW.h>
#include <Qt/RawAPI_Define.h>

using namespace Ubpa;

const float Raster::lightNear = 0.01f;
const float Raster::lightFar = 25.f;

Raster::Raster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
	: pOGLW(pOGLW),scene(scene),camera(camera),
	pldmGenerator(PLDM_Generator::New(pOGLW, lightNear, lightFar)),
	dldmGenerator(DLDM_Generator::New(pOGLW, camera)),
	sldmGenerator(SLDM_Generator::New(pOGLW, camera, lightNear, lightFar)),
	envGenerator(EnvGenerator::New(pOGLW))
{

}

void Raster::UpdateShadowMap() {
	pldmGenerator->Visit(scene);
	dldmGenerator->Visit(scene);
	sldmGenerator->Visit(scene);
}

void Raster::UpdateEnvironment() {
	envGenerator->Visit(scene);
}

void Raster::Init() {
	pldmGenerator->Init();
	dldmGenerator->Init();
	sldmGenerator->Init();
	envGenerator->Init();

	glGenBuffers(1, &pointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, pointLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 272, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, pointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &directionalLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, directionalLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 784, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, directionalLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &spotLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, spotLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 912, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, spotLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &environmentUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, environmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, 32, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, environmentUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &sphereLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, sphereLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 272, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 5, sphereLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &diskLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, diskLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 400, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, diskLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &areaLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, areaLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 528, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 7, areaLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &capsuleLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, capsuleLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 400, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 8, capsuleLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Raster::UpdateUBO() {
	UpdateUBO_PointLights();
	UpdateUBO_DirectionalLights();
	UpdateUBO_SpotLights();
	UpdateUBO_Environment();
	UpdateUBO_SphereLights();
	UpdateUBO_DiskLights();
	UpdateUBO_AreaLights();
	UpdateUBO_CapsuleLights();
}

void Raster::UpdateUBO_PointLights() {
	pointLight2idx.clear();

	int pointLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, pointLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto pointLight = CastTo<PointLight>(cmptLight->light);
		if (!pointLight)
			continue;

		pointLight2idx[pointLight] = pointLightIdx;

		pointf3 position = cmptLight->GetSObj()->GetWorldPos();

		int base = 16 + 32 * pointLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base +  0, 12, position.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 12,  4, &pointLight->radius);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, pointLight->IlluminancePower().data());

		pointLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &pointLightIdx); // 点光源个数即为 pointLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_DirectionalLights() {
	directionalLight2idx.clear();

	int directionalLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, directionalLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto directionalLight = CastTo<DirectionalLight>(cmptLight->light);
		if (!directionalLight)
			continue;

		directionalLight2idx[directionalLight] = directionalLightIdx;

		// normalized, point to light
		vecf3 dir = -(cmptLight->GetSObj()->GetLocalToWorldMatrix() * vecf3(0, -1, 0)).normalize();

		int base = 16 + 128 * directionalLightIdx;
		auto lightL = directionalLight->intensity * directionalLight->color;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, lightL.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, dir.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 64, dldmGenerator->GetProjView(cmptLight).data());

		directionalLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &directionalLightIdx); // 方向光个数即为 directionalLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_SpotLights() {
	spotLight2idx.clear();

	int spotLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, spotLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto spotLight = CastTo<SpotLight>(cmptLight->light);
		if (!spotLight)
			continue;

		spotLight2idx[spotLight] = spotLightIdx;

		const auto l2w = cmptLight->GetSObj()->GetLocalToWorldMatrix();
		pointf3 pos = l2w * pointf3(0.f);
		vecf3 dir = (l2w * vecf3(0, -1, 0)).normalize();
		auto cosHalfAngle = spotLight->CosHalfAngle();
		auto cosFalloffAngle = spotLight->CosFalloffAngle();

		int base = 16 + 112 * spotLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base +  0, 12, pos.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 12,  4, &spotLight->radius);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, dir.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28,  4, &cosHalfAngle);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 12, spotLight->IlluminancePower().data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 44,  4, &cosFalloffAngle);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 48, 64, sldmGenerator->GetProjView(cmptLight).data());

		spotLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &spotLightIdx); // 聚光灯个数即为 spotLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_Environment() {
	glBindBuffer(GL_UNIFORM_BUFFER, environmentUBO);

	auto environment = scene->GetInfiniteAreaLight();
	if (!environment) {
		auto color = rgbf(0.f);
		float intensity = 0;
		bool haveSkybox = false;
		bool haveEnvironment = false;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, color.data());
		glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &intensity);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 1, &haveSkybox);
		glBufferSubData(GL_UNIFORM_BUFFER, 20, 1, &haveEnvironment);
	}
	else {
		bool haveSkybox = environment->GetImg() != nullptr;
		bool haveEnvironment = true;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, environment->colorFactor.data());
		glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &environment->intensity);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 1, &haveSkybox);
		glBufferSubData(GL_UNIFORM_BUFFER, 20, 1, &haveEnvironment);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_SphereLights() {
	sphereLight2idx.clear();

	int sphereLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, sphereLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto sphereLight = CastTo<SphereLight>(cmptLight->light);
		if (!sphereLight)
			continue;

		sphereLight2idx[sphereLight] = sphereLightIdx;

		pointf3 position = cmptLight->GetSObj()->GetWorldPos();

		int base = 16 + 32 * sphereLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, position.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, sphereLight->Luminance().data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28, 4, &sphereLight->radius);

		sphereLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &sphereLightIdx); // 球光源个数即为 sphereLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_DiskLights() {
	diskLight2idx.clear();

	int diskLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, diskLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto diskLight = CastTo<DiskLight>(cmptLight->light);
		if (!diskLight)
			continue;

		diskLight2idx[diskLight] = diskLightIdx;

		const auto l2w = cmptLight->GetSObj()->GetLocalToWorldMatrix();
		auto pos = l2w * pointf3(0.f);
		auto dir = l2w * normalf(0, 1, 0);

		int base = 16 + 48 * diskLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, pos.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, dir.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 12, diskLight->Luminance().data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 44, 4, &diskLight->radius);

		diskLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &diskLightIdx); // 圆盘光源个数即为 diskLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_AreaLights() {
	areaLight2idx.clear();

	int areaLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, areaLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto areaLight = CastTo<AreaLight>(cmptLight->light);
		if (!areaLight)
			continue;

		areaLight2idx[areaLight] = areaLightIdx;

		const auto l2w = cmptLight->GetSObj()->GetLocalToWorldMatrix();
		auto pos = l2w * pointf3(0.f);
		auto dir = (l2w * normalf(0, 1, 0)).normalize();
		auto horizontal = (l2w * normalf(1, 0, 0)).normalize();

		int base = 16 + 64 * areaLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, pos.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 12, 4, &areaLight->width);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, dir.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28, 4, &areaLight->height);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 12, horizontal.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 48, 12, areaLight->Luminance().data());

		areaLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &areaLightIdx); // 面光源个数即为 areaLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_CapsuleLights() {
	capsuleLight2idx.clear();

	int capsuleLightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, capsuleLightsUBO);
	for (auto cmptLight : scene->GetCmptLights()) {
		auto capsuleLight = CastTo<CapsuleLight>(cmptLight->light);
		if (!capsuleLight)
			continue;

		capsuleLight2idx[capsuleLight] = capsuleLightIdx;

		const auto l2w = cmptLight->GetSObj()->GetLocalToWorldMatrix();
		auto midPos = l2w * pointf3(0.f);
		auto up = (l2w * normalf(0, 1, 0)).normalize();
		auto p0 = midPos + 0.5f * capsuleLight->height * up.cast_to<vecf3>();
		auto p1 = midPos - 0.5f * capsuleLight->height * up.cast_to<vecf3>();
		float height = (p1 - p0).norm();

		int base = 16 + 48 * capsuleLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base +  0, 12, p0.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 12,  4, &capsuleLight->radius);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, p1.data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28,  4, &height);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 12, capsuleLight->Luminance().data());

		capsuleLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &capsuleLightIdx); // 胶囊光源个数即为 capsuleLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::BindUBO(Shader & shader) {
	shader.UniformBlockBind("Camera", 0);
	shader.UniformBlockBind("PointLights", 1);
	shader.UniformBlockBind("DirectionalLights", 2);
	shader.UniformBlockBind("SpotLights", 3);
	shader.UniformBlockBind("Environment", 4);
	shader.UniformBlockBind("SphereLights", 5);
	shader.UniformBlockBind("DiskLights", 6);
	shader.UniformBlockBind("AreaLights", 7);
	shader.UniformBlockBind("CapsuleLights", 8);
}

/*
void Raster::Draw() {

}
*/