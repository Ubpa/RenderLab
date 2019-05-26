#include <CppUtil/Engine/ForwardRaster.h>

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

const int ForwardRaster::maxPointLights = 8;
const int ForwardRaster::maxDirectionalLights = 8;
const int ForwardRaster::maxSpotLights = 8;

const string rootPath = ROOT_PATH;

bool ForwardRaster::ShaderCompare::operator()(const OpenGL::Shader & lhs, const OpenGL::Shader & rhs) const {
	if (!lhs.IsValid())
		return true;

	if (!rhs.IsValid())
		return false;

	return lhs.GetID() < rhs.GetID();
}

ForwardRaster::ForwardRaster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
	: Raster(pOGLW, scene, camera) {
	RegMemberFunc<SObj>(&ForwardRaster::Visit);

	// primitive
	RegMemberFunc<Engine::Sphere>(&ForwardRaster::Visit);
	RegMemberFunc<Engine::Plane>(&ForwardRaster::Visit);
	RegMemberFunc<TriMesh>(&ForwardRaster::Visit);

	// bsdf
	RegMemberFunc<BSDF_Diffuse>(&ForwardRaster::Visit);
	RegMemberFunc<BSDF_Emission>(&ForwardRaster::Visit);
	RegMemberFunc<BSDF_Glass>(&ForwardRaster::Visit);
	RegMemberFunc<BSDF_Mirror>(&ForwardRaster::Visit);
	RegMemberFunc<BSDF_CookTorrance>(&ForwardRaster::Visit);
	RegMemberFunc<BSDF_MetalWorkflow>(&ForwardRaster::Visit);
	RegMemberFunc<BSDF_FrostedGlass>(&ForwardRaster::Visit);
}

void ForwardRaster::Draw() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpdateShadowMap();
	UpdateEnvironment();

	// 要放在深度图生成之后
	UpdateUBO();

	modelVec.clear();
	modelVec.push_back(Transform(1.f));
	scene->GetRoot()->Accept(This());

	DrawEnvironment();
}

void ForwardRaster::Init() {
	Raster::Init();

	InitShaders();
}

void ForwardRaster::InitShaders() {
	InitShader_Basic();
	InitShader_Skybox();
}

void ForwardRaster::InitShader_Basic() {
	shader_basic = Shader(rootPath + str_Basic_P3_vs, rootPath + str_Basic_fs);

	RegShader(shader_basic);
}

void ForwardRaster::InitShader_Skybox() {
	string vsPath = "data/shaders/Engine/Skybox/skybox.vs";
	string fsPath = "data/shaders/Engine/Skybox/skybox.fs";
	shader_skybox = Shader(rootPath + vsPath, rootPath + fsPath);

	RegShader(shader_skybox, 0);
}

void ForwardRaster::Visit(Ptr<SObj> sobj) {
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

void ForwardRaster::Visit(Ptr<Engine::Sphere> sphere) {
	curShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Sphere).Draw(curShader);
}

void ForwardRaster::Visit(Ptr<Engine::Plane> plane) {
	curShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Plane).Draw(curShader);
}

void ForwardRaster::Visit(Ptr<TriMesh> mesh) {
	curShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(mesh).Draw(curShader);
}

void ForwardRaster::UseLightTex(const Shader & shader) const {
	auto target = shader2depthmapBase.find(shader);
	if (target == shader2depthmapBase.cend()) {
		printf("ERROR::ForwardRaster::UseLightTex:\n"
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
	const int directionalLightBase = depthmapBase + maxPointLights;
	for (auto cmptLight : scene->GetCmptLights()) {
		auto directionalLight = CastTo<DirectionalLight>(cmptLight->light);
		auto target = directionalLight2idx.find(directionalLight);
		if (target == directionalLight2idx.cend())
			continue;
		const auto directionalLightIdx = target->second;

		dldmGenerator->GetDepthMap(cmptLight).Use(directionalLightBase + directionalLightIdx);
	}

	// spot light
	const int spotLightBase = directionalLightBase + maxDirectionalLights;
	for (auto cmptLight : scene->GetCmptLights()) {
		auto spotLight = CastTo<SpotLight>(cmptLight->light);
		auto target = spotLight2idx.find(spotLight);
		if (target == spotLight2idx.cend())
			continue;
		const auto spotLightIdx = target->second;

		sldmGenerator->GetDepthMap(cmptLight).Use(spotLightBase + spotLightIdx);
	}

	// environment
	auto environment = scene->GetInfiniteAreaLight();
	const int environmentBase = spotLightBase + maxSpotLights;
	if (environment) {
		auto brdfLUT = envGenerator->GetBRDF_LUT();
		brdfLUT.Use(environmentBase + 3);

		if (environment->GetImg()) {
			auto skybox = envGenerator->GetSkybox(environment->GetImg());
			skybox.Use(environmentBase);
			auto irradianceMap = envGenerator->GetIrradianceMap(environment->GetImg());
			irradianceMap.Use(environmentBase + 1);
			auto prefilterMap = envGenerator->GetPrefilterMap(environment->GetImg());
			prefilterMap.Use(environmentBase + 2);
		}
	}
}

void ForwardRaster::RegShader(OpenGL::Shader & shader, int depthmapBase) {
	BindUBO(shader);

	if (depthmapBase < 0) // 无需计算光
		return;

	shader2depthmapBase[shader] = depthmapBase;

	shader.SetFloat("lightNear", lightNear);
	shader.SetFloat("lightFar", lightFar);

	// point light
	int pointLightBase = depthmapBase;
	for (int i = 0; i < maxPointLights; i++)
		shader.SetInt("pointLightDepthMap" + to_string(i), pointLightBase + i);

	// directional light
	int directionalLightBase = pointLightBase + maxPointLights;
	for (int i = 0; i < maxDirectionalLights; i++)
		shader.SetInt("directionalLightDepthMap" + to_string(i), directionalLightBase + i);

	// spot light
	int spotLightBase = directionalLightBase + maxDirectionalLights;
	for (int i = 0; i < maxSpotLights; i++)
		shader.SetInt("spotLightDepthMap" + to_string(i), spotLightBase + i);

	// environment
	const int environmentBase = spotLightBase + maxSpotLights;
	shader.SetInt("skybox", environmentBase);
	shader.SetInt("irradianceMap", environmentBase + 1);
	shader.SetInt("prefilterMap", environmentBase + 2);
	shader.SetInt("brdfLUT", environmentBase + 3);
}

void ForwardRaster::DrawEnvironment() {
	auto environment = scene->GetInfiniteAreaLight();
	if (!environment)
		return;

	glDepthFunc(GL_LEQUAL);
	UseLightTex(shader_skybox);
	pOGLW->GetVAO(ShapeType::Cube).Draw(shader_skybox);
	glDepthFunc(GL_LESS);
}
