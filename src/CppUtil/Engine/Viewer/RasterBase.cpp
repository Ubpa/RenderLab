#include <CppUtil/Engine/RasterBase.h>

#include "DLDM_Generator.h"
#include "PLDM_Generator.h"
#include "SLDM_Generator.h"
#include "EnvGenerator.h"

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptMaterial.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/PointLight.h>
#include <CppUtil/Engine/DirectionalLight.h>
#include <CppUtil/Engine/SpotLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/Camera.h>
#include <CppUtil/OpenGL/FBO.h>

#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/Sphere.h>
#include <CppUtil/Basic/Plane.h>
#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

using namespace CppUtil::Engine;
using namespace CppUtil::QT;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace std;

const int RasterBase::maxPointLights = 8;
const int RasterBase::maxDirectionalLights = 8;
const int RasterBase::maxSpotLights = 8;

const float RasterBase::lightNear = 0.01f;
const float RasterBase::lightFar = 25.f;

const string rootPath = ROOT_PATH;

bool RasterBase::ShaderCompare::operator()(const OpenGL::Shader & lhs, const OpenGL::Shader & rhs) const {
	if (!lhs.IsValid())
		return true;

	if (!rhs.IsValid())
		return false;

	return lhs.GetID() < rhs.GetID();
}

RasterBase::RasterBase(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
	: pOGLW(pOGLW), scene(scene),
	pldmGenerator(PLDM_Generator::New(pOGLW, lightNear, lightFar)),
	dldmGenerator(DLDM_Generator::New(pOGLW, camera)),
	sldmGenerator(SLDM_Generator::New(pOGLW, camera, lightNear, lightFar)),
	cmGenerator(EnvGenerator::New(pOGLW)) {
	RegMemberFunc<SObj>(&RasterBase::Visit);

	// primitive
	RegMemberFunc<Engine::Sphere>(&RasterBase::Visit);
	RegMemberFunc<Engine::Plane>(&RasterBase::Visit);
	RegMemberFunc<TriMesh>(&RasterBase::Visit);

	// bsdf
	RegMemberFunc<BSDF_Diffuse>(&RasterBase::Visit);
	RegMemberFunc<BSDF_Emission>(&RasterBase::Visit);
	RegMemberFunc<BSDF_Glass>(&RasterBase::Visit);
	RegMemberFunc<BSDF_Mirror>(&RasterBase::Visit);
	RegMemberFunc<BSDF_CookTorrance>(&RasterBase::Visit);
	RegMemberFunc<BSDF_MetalWorkflow>(&RasterBase::Visit);
	RegMemberFunc<BSDF_FrostedGlass>(&RasterBase::Visit);
}

void RasterBase::Draw() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene->Accept(pldmGenerator);
	scene->Accept(dldmGenerator);
	scene->Accept(sldmGenerator);
	scene->Accept(cmGenerator);

	// 要放在深度图生成之后
	UpdateLights();

	modelVec.clear();
	modelVec.push_back(Transform(1.f));
	scene->GetRoot()->Accept(This());

	DrawEnvironment();
}

void RasterBase::OGL_Init() {
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


	pldmGenerator->OGL_Init();
	dldmGenerator->OGL_Init();
	sldmGenerator->OGL_Init();
	cmGenerator->OGL_Init();

	InitShaders();
}

void RasterBase::InitShaders() {
	InitShader_Basic();
	InitShader_Skybox();
}

void RasterBase::InitShader_Basic() {
	shader_basic = Shader(rootPath + str_Basic_P3_vs, rootPath + str_Basic_fs);

	RegShader(shader_basic);
}

void RasterBase::InitShader_Skybox() {
	string vsPath = "data/shaders/Engine/Skybox/skybox.vs";
	string fsPath = "data/shaders/Engine/Skybox/skybox.fs";
	shader_skybox = Shader(rootPath + vsPath, rootPath + fsPath);

	RegShader(shader_skybox, 0);
}

void RasterBase::Visit(Ptr<SObj> sobj) {
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto material = sobj->GetComponent<CmptMaterial>();
	auto children = sobj->GetChildren();

	// 这种情况下不需要 transform
	if ((!geometry || !geometry->primitive || !material || !material->material)
		&& children.size() == 0)
		return;

	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform != nullptr)
		modelVec.push_back(modelVec.back() * cmptTransform->GetTransform());

	if (geometry && geometry->primitive && material && material->material) {
		material->material->Accept(This());
		geometry->primitive->Accept(This());
	}

	for (auto child : children)
		child->Accept(This());

	if (cmptTransform != nullptr)
		modelVec.pop_back();
}

void RasterBase::Visit(Ptr<Engine::Sphere> sphere) {
	curShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Sphere).Draw(curShader);
}

void RasterBase::Visit(Ptr<Engine::Plane> plane) {
	curShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Plane).Draw(curShader);
}

void RasterBase::Visit(Ptr<TriMesh> mesh) {
	curShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(mesh).Draw(curShader);
}

void RasterBase::UpdateLights() {
	UpdatePointLights();
	UpdateDirectionalLights();
	UpdateSpotLights();
	UpdateEnvironment();
}

void RasterBase::UpdatePointLights() {
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

void RasterBase::UpdateDirectionalLights() {
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

void RasterBase::UpdateSpotLights() {
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

void RasterBase::UpdateEnvironment() {
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

void RasterBase::UseLightTex(const Shader & shader) const {
	auto target = shader2depthmapBase.find(shader);
	if (target == shader2depthmapBase.cend()) {
		printf("ERROR::RasterBase::UseLightTex:\n"
			"\t""shader not regist\n");
		return;
	}
	const auto depthmapBase = target->second;


	// point light
	for (auto cmptLight : scene->GetCmptLights()) {
		auto pointLight = CastTo<PointLight>(cmptLight->light);
		auto target = pointLight2idx.find(pointLight);
		if (target == pointLight2idx.cend())
			continue;
		const auto pointLightIdx = target->second;

		pldmGenerator->GetDepthCubeMap(cmptLight).Use(depthmapBase + pointLightIdx);
	}

	// directional light
	for (auto cmptLight : scene->GetCmptLights()) {
		auto directionalLight = CastTo<DirectionalLight>(cmptLight->light);
		auto target = directionalLight2idx.find(directionalLight);
		if (target == directionalLight2idx.cend())
			continue;
		const auto directionalLightIdx = target->second;

		dldmGenerator->GetDepthMap(cmptLight).Use(depthmapBase + maxPointLights + directionalLightIdx);
	}

	// spot light
	for (auto cmptLight : scene->GetCmptLights()) {
		auto spotLight = CastTo<SpotLight>(cmptLight->light);
		auto target = spotLight2idx.find(spotLight);
		if (target == spotLight2idx.cend())
			continue;
		const auto spotLightIdx = target->second;

		sldmGenerator->GetDepthMap(cmptLight).Use(depthmapBase + maxPointLights + maxDirectionalLights + spotLightIdx);
	}

	// environment
	auto environment = scene->GetInfiniteAreaLight();
	if (!environment || !environment->GetImg())
		return;
	auto skybox = cmGenerator->GetSkybox(environment->GetImg());
	skybox.Use(depthmapBase + maxPointLights + maxDirectionalLights + maxSpotLights);
	auto irradianceMap = cmGenerator->GetIrradianceMap(environment->GetImg());
	irradianceMap.Use(depthmapBase + maxPointLights + maxDirectionalLights + maxSpotLights + 1);
}

void RasterBase::RegShader(const OpenGL::Shader & shader, int depthmapBase) {
	shader.UniformBlockBind("Camera", 0);

	if (depthmapBase < 0) // 无需计算光
		return;

	shader2depthmapBase[shader] = depthmapBase;

	shader.SetFloat("lightNear", lightNear);
	shader.SetFloat("lightFar", lightFar);

	// point light
	shader.UniformBlockBind("PointLights", 1);

	int pointLightBase = depthmapBase;
	for (int i = 0; i < maxPointLights; i++)
		shader.SetInt("pointLightDepthMap" + to_string(i), pointLightBase + i);

	// directional light
	shader.UniformBlockBind("DirectionalLights", 2);

	int directionalLightBase = pointLightBase + maxPointLights;
	for (int i = 0; i < maxDirectionalLights; i++)
		shader.SetInt("directionalLightDepthMap" + to_string(i), directionalLightBase + i);

	// spot light
	shader.UniformBlockBind("SpotLights", 3);

	int spotLightBase = directionalLightBase + maxDirectionalLights;
	for (int i = 0; i < maxSpotLights; i++)
		shader.SetInt("spotLightDepthMap" + to_string(i), depthmapBase + maxPointLights + maxDirectionalLights + i);

	// environment
	shader.UniformBlockBind("Environment", 4);

	int environmentBase = spotLightBase + maxSpotLights;
	shader.SetInt("skybox", environmentBase);
	shader.SetInt("irradianceMap", environmentBase + 1);
}

void RasterBase::DrawEnvironment() {
	auto environment = scene->GetInfiniteAreaLight();
	if (!environment)
		return;

	glDepthFunc(GL_LEQUAL);
	UseLightTex(shader_skybox);
	pOGLW->GetVAO(ShapeType::Cube).Draw(shader_skybox);
	glDepthFunc(GL_LESS);
}
