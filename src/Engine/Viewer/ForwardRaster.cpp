#include <Engine/Viewer/ForwardRaster.h>

#include "DLDM_Generator.h"
#include "PLDM_Generator.h"
#include "SLDM_Generator.h"
#include "EnvGenerator.h"

#include <Engine/Scene/Scene.h>
#include <Engine/Scene/SObj.h>

#include <Engine/Scene/CmptMaterial.h>

#include <Engine/Scene/CmptTransform.h>

#include <Engine/Scene/CmptGeometry.h>
#include <Engine/Primitive/Sphere.h>
#include <Engine/Primitive/Plane.h>
#include <Engine/Primitive/TriMesh.h>

#include <Engine/Material/AllBSDFs.h>
#include <Engine/Material/Gooch.h>

#include <Engine/Scene/CmptLight.h>
#include <Engine/Light/PointLight.h>
#include <Engine/Light/DirectionalLight.h>
#include <Engine/Light/SpotLight.h>
#include <Engine/Light/InfiniteAreaLight.h>

#include <Qt/RawAPI_OGLW.h>
#include <Qt/RawAPI_Define.h>

#include <OpenGL/VAO.h>
#include <OpenGL/CommonDefine.h>
#include <OpenGL/Texture.h>
#include <OpenGL/Shader.h>
#include <OpenGL/Camera.h>
#include <OpenGL/FBO.h>

#include <Basic/Op/LambdaOp.h>
#include <Basic/Image.h>

#include <ROOT_PATH.h>

using namespace Ubpa;

using namespace Define;
using namespace std;

const int ForwardRaster::maxPointLights = 8;
const int ForwardRaster::maxDirectionalLights = 8;
const int ForwardRaster::maxSpotLights = 8;

const string rootPath = ROOT_PATH;

bool ForwardRaster::ShaderCompare::operator()(const Shader & lhs, const Shader & rhs) const {
	if (!lhs.IsValid())
		return true;

	if (!rhs.IsValid())
		return false;

	return lhs.GetID() < rhs.GetID();
}

ForwardRaster::ForwardRaster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
	: Raster(pOGLW, scene, camera)
{
	// primitive
	Regist<Sphere, Plane, TriMesh>();

	// bsdf
	Regist<BSDF_Diffuse,
		BSDF_Emission,
		BSDF_Glass,
		BSDF_Mirror,
		BSDF_CookTorrance,
		BSDF_MetalWorkflow,
		BSDF_FrostedGlass,
		BSDF_Frostbite,
		Gooch>();
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
	modelVec.push_back(transformf::eye());
	Visit(scene->GetRoot());

	if(drawSky)
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
	shader_basic.SetVecf3("color", valf3(1.f,1.f,1.f));
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
		Visit(material->material);
		Visit(geometry->primitive);
	}

	for (auto child : children)
		Visit(child);

	if (cmptTransform != nullptr)
		modelVec.pop_back();
}

void ForwardRaster::ImplVisit(Ptr<Sphere> sphere) {
	curShader.SetMatf4("model", modelVec.back().data());
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Sphere).Draw(curShader);
}

void ForwardRaster::ImplVisit(Ptr<Plane> plane) {
	curShader.SetMatf4("model", modelVec.back().data());
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Plane).Draw(curShader);
}

void ForwardRaster::ImplVisit(Ptr<TriMesh> mesh) {
	curShader.SetMatf4("model", modelVec.back().data());
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

void ForwardRaster::RegShader(Shader & shader, int depthmapBase) {
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
	shader_skybox.SetBool("needGamma", true);
	pOGLW->GetVAO(RawAPI_OGLW::ShapeType::Cube).Draw(shader_skybox);
	glDepthFunc(GL_LESS);
}

void ForwardRaster::ImplVisit(Ptr<BSDF_Diffuse> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->colorFactor.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_Glass> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->transmittance.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_Mirror> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->reflectance.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_Emission> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->color.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_CookTorrance> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->albedo.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_MetalWorkflow> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->colorFactor.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_FrostedGlass> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->colorFactor.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<BSDF_Frostbite> bsdf) {
	curShader = shader_basic;
	curShader.SetVecf3("color", bsdf->colorFactor.cast_to<valf3>());
}
void ForwardRaster::ImplVisit(Ptr<Gooch> gooch) {
	curShader = shader_basic;
	curShader.SetVecf3("color", gooch->colorFactor.cast_to<valf3>());
}