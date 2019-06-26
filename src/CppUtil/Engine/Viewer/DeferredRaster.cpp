#include <CppUtil/Engine/DeferredRaster.h>

#include "DLDM_Generator.h"
#include "PLDM_Generator.h"
#include "SLDM_Generator.h"
#include "EnvGenerator.h"

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/CmptTransform.h>
#include <CppUtil/Engine/CmptMaterial.h>
#include <CppUtil/Engine/CmptLight.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/Disk.h>

#include <CppUtil/Engine/PointLight.h>
#include <CppUtil/Engine/DirectionalLight.h>
#include <CppUtil/Engine/SpotLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>

// support material
#include <CppUtil/Engine/BSDF_MetalWorkflow.h>
#include <CppUtil/Engine/BSDF_Diffuse.h>
#include <CppUtil/Engine/BSDF_Frostbite.h>
#include <CppUtil/Engine/BSDF_Emission.h>

#include <CppUtil/OpenGL/CommonDefine.h>

#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

using namespace Define;
using namespace std;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::QT;

const int DeferredRaster::maxPointLights = 8;
const int DeferredRaster::maxDirectionalLights = 8;
const int DeferredRaster::maxSpotLights = 8;

const string rootPath = ROOT_PATH;

DeferredRaster::DeferredRaster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
	: Raster(pOGLW,scene,camera), curMaterialShader(Shader::InValid)
{
	RegMemberFunc<SObj>(&DeferredRaster::Visit);

	// primitive
	RegMemberFunc<Sphere>(&DeferredRaster::Visit);
	RegMemberFunc<Plane>(&DeferredRaster::Visit);
	RegMemberFunc<TriMesh>(&DeferredRaster::Visit);
	RegMemberFunc<Disk>(&DeferredRaster::Visit);

	// material
	RegMemberFunc<BSDF_MetalWorkflow>(&DeferredRaster::Visit);
	RegMemberFunc<BSDF_Diffuse>(&DeferredRaster::Visit);
	RegMemberFunc<BSDF_Frostbite>(&DeferredRaster::Visit);
	RegMemberFunc<BSDF_Emission>(&DeferredRaster::Visit);

	// regist ID
	mngrMID.Reg<BSDF_MetalWorkflow>(0);
	mngrMID.Reg<BSDF_Diffuse>(1);
	mngrMID.Reg<BSDF_Frostbite>(2);
	mngrMID.Reg<BSDF_Emission>(3);
}

void DeferredRaster::Init() {
	Raster::Init();

	InitShaders();

	gbufferFBO = FBO(pOGLW->width(), pOGLW->height(), { 4,4,4,4 });
	windowFBO = FBO(pOGLW->width(), pOGLW->height(), {3});
}

void DeferredRaster::InitShaders() {
	InitShader_GBuffer();
	InitShader_DirectLight();
	InitShader_AmbientLight();
	InitShader_Skybox();
	InitShader_PostProcess();
}

void DeferredRaster::InitShader_GBuffer() {
	InitShader_GBuffer_MetalWorkflow();
	InitShader_GBuffer_Diffuse();
	InitShader_GBuffer_Frostbite();
	InitShader_GBuffer_Emission();
}

void DeferredRaster::InitShader_GBuffer_MetalWorkflow() {
	metalShader = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/DeferredPipeline/GBuffer_MetalWorkflow.fs");

	metalShader.SetInt("metal.albedoTexture", 0);
	metalShader.SetInt("metal.metallicTexture", 1);
	metalShader.SetInt("metal.roughnessTexture", 2);
	metalShader.SetInt("metal.aoTexture", 3);
	metalShader.SetInt("metal.normalTexture", 4);

	metalShader.SetInt("ID", mngrMID.Get<BSDF_MetalWorkflow>());

	BindUBO(metalShader);
}

void DeferredRaster::InitShader_GBuffer_Diffuse() {
	diffuseShader = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/DeferredPipeline/GBuffer_Diffuse.fs");

	diffuseShader.SetInt("diffuse.albedoTexture", 0);

	diffuseShader.SetInt("ID", mngrMID.Get<BSDF_Diffuse>());
	
	BindUBO(diffuseShader);
}

void DeferredRaster::InitShader_GBuffer_Frostbite() {
	frostbiteShader = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/DeferredPipeline/GBuffer_Frostbite.fs");

	frostbiteShader.SetInt("bsdf.albedoTexture", 0);
	frostbiteShader.SetInt("bsdf.metallicTexture", 1);
	frostbiteShader.SetInt("bsdf.roughnessTexture", 2);
	frostbiteShader.SetInt("bsdf.aoTexture", 3);
	frostbiteShader.SetInt("bsdf.normalTexture", 4);

	frostbiteShader.SetInt("ID", mngrMID.Get<BSDF_Frostbite>());

	BindUBO(frostbiteShader);
}

void DeferredRaster::InitShader_GBuffer_Emission() {
	emissionShader = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/DeferredPipeline/GBuffer_Emission.fs");
	emissionShader.SetInt("ID", mngrMID.Get<BSDF_Emission>());

	BindUBO(emissionShader);
}

void DeferredRaster::InitShader_DirectLight() {
	directLightShader = Shader(rootPath + str_Screen_vs, rootPath + "data/shaders/Engine/DeferredPipeline/directLight.fs");

	int idx = 0;

	directLightShader.SetInt("GBuffer0", idx++);
	directLightShader.SetInt("GBuffer1", idx++);
	directLightShader.SetInt("GBuffer2", idx++);
	directLightShader.SetInt("GBuffer3", idx++);

	for (int i = 0; i < maxPointLights; i++)
		directLightShader.SetInt("pointLightDepthMap" + to_string(i), idx++);
	for (int i = 0; i < maxDirectionalLights; i++)
		directLightShader.SetInt("directionalLightDepthMap" + to_string(i), idx++);
	for (int i = 0; i < maxSpotLights; i++)
		directLightShader.SetInt("spotLightDepthMap" + to_string(i), idx++);

	directLightShader.SetFloat("lightNear", lightNear);
	directLightShader.SetFloat("lightFar", lightFar);

	BindUBO(directLightShader);
}

void DeferredRaster::InitShader_AmbientLight() {
	ambientLightShader = Shader(rootPath + str_Screen_vs, rootPath + "data/shaders/Engine/DeferredPipeline/ambientLight.fs");

	int idx = 0;

	ambientLightShader.SetInt("DirectLight", idx++);

	ambientLightShader.SetInt("GBuffer0", idx++);
	ambientLightShader.SetInt("GBuffer1", idx++);
	ambientLightShader.SetInt("GBuffer2", idx++);
	ambientLightShader.SetInt("GBuffer3", idx++);

	ambientLightShader.SetInt("irradianceMap", idx++);
	ambientLightShader.SetInt("prefilterMap", idx++);
	ambientLightShader.SetInt("brdfLUT", idx++);

	BindUBO(ambientLightShader);
}

void DeferredRaster::InitShader_Skybox() {
	string vsPath = "data/shaders/Engine/Skybox/skybox.vs";
	string fsPath = "data/shaders/Engine/Skybox/skybox.fs";
	skyboxShader = Shader(rootPath + vsPath, rootPath + fsPath);

	skyboxShader.SetInt("skybox", 0);

	BindUBO(skyboxShader);
}

void DeferredRaster::InitShader_PostProcess() {
	postProcessShader = Shader(rootPath + str_Screen_vs, rootPath + "data/shaders/Engine/DeferredPipeline/postProcess.fs");
	
	postProcessShader.SetInt("img", 0);
}

void DeferredRaster::Resize() {
	gbufferFBO.Resize(pOGLW->w, pOGLW->h);

	windowFBO.Resize(pOGLW->w, pOGLW->h);
}

void DeferredRaster::Draw() {
	glEnable(GL_DEPTH_TEST);

	UpdateShadowMap();
	UpdateEnvironment();

	UpdateUBO();

	Pass_GBuffer();

	glDisable(GL_DEPTH_TEST);
	Pass_DirectLight();
	Pass_AmbientLight();
	glEnable(GL_DEPTH_TEST);

	Pass_Skybox();
	Pass_PostProcess();
}

void DeferredRaster::Pass_GBuffer() {
	gbufferFBO.Use();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelVec.clear();
	modelVec.push_back(Transform(1.f));
	scene->GetRoot()->Accept(This());
}

void DeferredRaster::Pass_DirectLight() {
	windowFBO.Use();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// set gbuffer
	gbufferFBO.GetColorTexture(0).Use(0);
	gbufferFBO.GetColorTexture(1).Use(1);
	gbufferFBO.GetColorTexture(2).Use(2);
	gbufferFBO.GetColorTexture(3).Use(3);

	// set point light
	for (auto cmptLight : scene->GetCmptLights()) {
		auto pointLight = CastTo<PointLight>(cmptLight->light);
		auto target = pointLight2idx.find(pointLight);
		if (target == pointLight2idx.cend())
			continue;
		const auto pointLightIdx = target->second;

		pldmGenerator->GetDepthCubeMap(cmptLight).Use(4 + pointLightIdx);
	}

	// set directional light
	const int directionalLightBase = 4 + maxPointLights;
	for (auto cmptLight : scene->GetCmptLights()) {
		auto directionalLight = CastTo<DirectionalLight>(cmptLight->light);
		auto target = directionalLight2idx.find(directionalLight);
		if (target == directionalLight2idx.cend())
			continue;
		const auto directionalLightIdx = target->second;

		dldmGenerator->GetDepthMap(cmptLight).Use(directionalLightBase + directionalLightIdx);
	}

	// set spot light
	const int spotLightBase = directionalLightBase + maxDirectionalLights;
	for (auto cmptLight : scene->GetCmptLights()) {
		auto spotLight = CastTo<SpotLight>(cmptLight->light);
		auto target = spotLight2idx.find(spotLight);
		if (target == spotLight2idx.cend())
			continue;
		const auto spotLightIdx = target->second;

		sldmGenerator->GetDepthMap(cmptLight).Use(spotLightBase + spotLightIdx);
	}

	pOGLW->GetVAO(ShapeType::Screen).Draw(directLightShader);
}

void DeferredRaster::Pass_AmbientLight() {
	windowFBO.Use();

	windowFBO.GetColorTexture(0).Use(0);

	// set gbuffer
	gbufferFBO.GetColorTexture(0).Use(1);
	gbufferFBO.GetColorTexture(1).Use(2);
	gbufferFBO.GetColorTexture(2).Use(3);
	gbufferFBO.GetColorTexture(3).Use(4);

	// set environment
	auto environment = scene->GetInfiniteAreaLight();
	const int environmentBase = 5;
	if (environment) {
		if (environment->GetImg()) {
			auto irradianceMap = envGenerator->GetIrradianceMap(environment->GetImg());
			irradianceMap.Use(environmentBase);
			auto prefilterMap = envGenerator->GetPrefilterMap(environment->GetImg());
			prefilterMap.Use(environmentBase + 1);
		}

		auto brdfLUT = envGenerator->GetBRDF_LUT();
		brdfLUT.Use(environmentBase + 2);
	}

	pOGLW->GetVAO(ShapeType::Screen).Draw(ambientLightShader);
}

void DeferredRaster::Pass_Skybox() {
	gbufferFBO.PassTo(windowFBO, FBO::ENUM_PASS_DEPTH);

	auto environment = scene->GetInfiniteAreaLight();
	if (!environment)
		return;

	windowFBO.Use();

	auto img = environment->GetImg();
	if (img)
		envGenerator->GetSkybox(img).Use(0);

	glDepthFunc(GL_LEQUAL);
	pOGLW->GetVAO(ShapeType::Cube).Draw(skyboxShader);
	glDepthFunc(GL_LESS);
}

void DeferredRaster::Pass_PostProcess() {
	FBO::UseDefault();
	windowFBO.GetColorTexture(0).Use(0);
	pOGLW->GetVAO(ShapeType::Screen).Draw(postProcessShader);
}

void DeferredRaster::Visit(Ptr<SObj> sobj) {
	auto geometry = sobj->GetComponent<CmptGeometry>();
	auto children = sobj->GetChildren();

	auto cmptTransform = sobj->GetComponent<CmptTransform>();
	if (cmptTransform != nullptr)
		modelVec.push_back(modelVec.back() * cmptTransform->GetTransform());

	do {
		if (!geometry || !geometry->primitive)
			break;

		auto cmptMaterial = sobj->GetComponent<CmptMaterial>();
		if (!cmptMaterial || !cmptMaterial->material)
			break;

		curMaterialShader = Shader::InValid;
		cmptMaterial->material->Accept(This());

		geometry->primitive->Accept(This());
	} while (false);

	for (auto child : children)
		child->Accept(This());

	if (cmptTransform != nullptr)
		modelVec.pop_back();
}

void DeferredRaster::Visit(Ptr<BSDF_MetalWorkflow> metal) {
	string strBSDF = "metal.";
	metalShader.SetVec3f(strBSDF + "colorFactor", metal->colorFactor);
	metalShader.SetFloat(strBSDF + "metallicFactor", metal->metallicFactor);
	metalShader.SetFloat(strBSDF + "roughnessFactor", metal->roughnessFactor);

	const int texNum = 5;
	PtrC<Image> imgs[texNum] = { metal->albedoTexture, metal->metallicTexture, metal->roughnessTexture, metal->aoTexture, metal->normalTexture };
	string names[texNum] = { "Albedo", "Metallic", "Roughness", "AO", "Normal" };

	for (int i = 0; i < texNum; i++) {
		string wholeName = strBSDF + "have" + names[i] + "Texture";
		if (imgs[i] && imgs[i]->IsValid()) {
			metalShader.SetBool(wholeName, true);
			pOGLW->GetTex(imgs[i]).Use(i);
		}
		else
			metalShader.SetBool(wholeName, false);
	}

	curMaterialShader = metalShader;
}

void DeferredRaster::Visit(Ptr<BSDF_Diffuse> diffuse) {
	string strBSDF = "diffuse.";
	diffuseShader.SetVec3f(strBSDF + "colorFactor", diffuse->colorFactor);
	if (diffuse->albedoTexture && diffuse->albedoTexture->IsValid()) {
		diffuseShader.SetBool(strBSDF + "haveAlbedoTexture", true);
		pOGLW->GetTex(diffuse->albedoTexture).Use(0);
	}
	else
		diffuseShader.SetBool(strBSDF + "haveAlbedoTexture", false);

	curMaterialShader = diffuseShader;
}

void DeferredRaster::Visit(Ptr<BSDF_Frostbite> bsdf) {
	string strBSDF = "bsdf.";
	frostbiteShader.SetVec3f(strBSDF + "colorFactor", bsdf->colorFactor);
	frostbiteShader.SetFloat(strBSDF + "metallicFactor", bsdf->metallicFactor);
	frostbiteShader.SetFloat(strBSDF + "roughnessFactor", bsdf->roughnessFactor);

	const int texNum = 5;
	PtrC<Image> imgs[texNum] = { bsdf->albedoTexture, bsdf->metallicTexture, bsdf->roughnessTexture, bsdf->aoTexture, bsdf->normalTexture };
	string names[texNum] = { "Albedo", "Metallic", "Roughness", "AO", "Normal" };

	for (int i = 0; i < texNum; i++) {
		string wholeName = strBSDF + "have" + names[i] + "Texture";
		if (imgs[i] && imgs[i]->IsValid()) {
			frostbiteShader.SetBool(wholeName, true);
			pOGLW->GetTex(imgs[i]).Use(i);
		}
		else
			frostbiteShader.SetBool(wholeName, false);
	}

	curMaterialShader = frostbiteShader;
}

void DeferredRaster::Visit(Ptr<BSDF_Emission> emission) {
	emissionShader.SetVec3f("emission.L", emission->intensity * emission->color);

	curMaterialShader = emissionShader;
}

void DeferredRaster::Visit(Ptr<Sphere> sphere) {
	if (!curMaterialShader.IsValid())
		return;

	curMaterialShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Sphere).Draw(curMaterialShader);
}

void DeferredRaster::Visit(Ptr<Plane> plane) {
	if (!curMaterialShader.IsValid())
		return;

	curMaterialShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Plane).Draw(curMaterialShader);
}

void DeferredRaster::Visit(Ptr<TriMesh> mesh) {
	if (!curMaterialShader.IsValid())
		return;

	curMaterialShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(mesh).Draw(curMaterialShader);
}

void DeferredRaster::Visit(Ptr<Disk> disk) {
	if (!curMaterialShader.IsValid())
		return;

	curMaterialShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Disk).Draw(curMaterialShader);
}
