#include <CppUtil/Engine/Raster.h>

#include "DLDM_Generator.h"
#include "PLDM_Generator.h"
#include "SLDM_Generator.h"
#include "EnvGenerator.h"

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/PointLight.h>
#include <CppUtil/Engine/DirectionalLight.h>
#include <CppUtil/Engine/SpotLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

using namespace CppUtil::Basic;
using namespace CppUtil::QT;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Engine;

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
	scene->Accept(pldmGenerator);
	scene->Accept(dldmGenerator);
	scene->Accept(sldmGenerator);
}

void Raster::UpdateEnvironment() {
	scene->Accept(envGenerator);
}

void Raster::Init() {
	pldmGenerator->Init();
	dldmGenerator->Init();
	sldmGenerator->Init();
	envGenerator->Init();

	glGenBuffers(1, &pointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, pointLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 400, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, pointLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &directionalLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, directionalLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 784, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, directionalLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &spotLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, spotLightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 1040, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, spotLightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &environmentUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, environmentUBO);
	glBufferData(GL_UNIFORM_BUFFER, 32, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 4, environmentUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO() {
	UpdateUBO_PointLights();
	UpdateUBO_DirectionalLights();
	UpdateUBO_SpotLights();
	UpdateUBO_Environment();
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

		Point3 position = cmptLight->GetSObj()->GetWorldPos();

		int base = 16 + 48 * pointLightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, position.Data());
		auto lightL = pointLight->intensity * pointLight->color;
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, lightL.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28, 4, &pointLight->linear);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 4, &pointLight->quadratic);

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

		Vec3f dir = cmptLight->GetSObj()->GetLocalToWorldMatrix()(Normalf(0, -1, 0));

		int base = 16 + 128 * directionalLightIdx;
		auto lightL = directionalLight->intensity * directionalLight->color;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, lightL.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, dir.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 64, dldmGenerator->GetProjView(cmptLight).GetMatrix().Data());

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
		Point3 pos = l2w(Point3(0.f));
		Vec3f dir = l2w(Normalf(0, -1, 0));

		int base = 16 + 64 * spotLightIdx;
		auto lightL = spotLight->intensity * spotLight->color;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, pos.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, dir.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 12, spotLight->GetMaxL().Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 48, 64, sldmGenerator->GetProjView(cmptLight).GetMatrix().Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 112, 4, &spotLight->linear);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 116, 4, &spotLight->quadratic);
		auto cosHalfAngle = spotLight->CosHalfAngle();
		auto cosFalloffAngle = spotLight->CosFalloffAngle();
		glBufferSubData(GL_UNIFORM_BUFFER, base + 120, 4, &cosHalfAngle);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 124, 4, &cosFalloffAngle);

		spotLightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &spotLightIdx); // 聚光灯个数即为 spotLightIdx
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::UpdateUBO_Environment() {
	glBindBuffer(GL_UNIFORM_BUFFER, environmentUBO);

	auto environment = scene->GetInfiniteAreaLight();
	if (!environment) {
		auto color = RGBf(0.f);
		float intensity = 0;
		bool haveSkybox = false;
		bool haveEnvironment = false;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, color.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &intensity);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 1, &haveSkybox);
		glBufferSubData(GL_UNIFORM_BUFFER, 20, 1, &haveEnvironment);
	}
	else {
		bool haveSkybox = environment->GetImg() != nullptr;
		bool haveEnvironment = true;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 12, environment->colorFactor.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &environment->intensity);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 1, &haveSkybox);
		glBufferSubData(GL_UNIFORM_BUFFER, 20, 1, &haveEnvironment);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Raster::BindUBO(Shader & shader) {
	shader.UniformBlockBind("Camera", 0);
	shader.UniformBlockBind("PointLights", 1);
	shader.UniformBlockBind("DirectionalLights", 2);
	shader.UniformBlockBind("SpotLights", 3);
	shader.UniformBlockBind("Environment", 4);
}

/*
void Raster::Draw() {

}
*/