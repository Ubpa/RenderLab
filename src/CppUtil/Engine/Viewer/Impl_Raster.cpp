#include "Impl_Raster.h"

#include "PLDM_Generator.h"

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Geometry.h>
#include <CppUtil/Engine/Material.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/AllBSDFs.h>

#include <CppUtil/Engine/Light.h>
#include <CppUtil/Engine/AreaLight.h>
#include <CppUtil/Engine/PointLight.h>

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
using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace glm;
using namespace std;

const string rootPath = ROOT_PATH;
const int Impl_Raster::maxPointLights = 8;

Impl_Raster::Impl_Raster(Scene::Ptr scene)
	: scene(scene), pldmGenerator(ToPtr(new PLDM_Generator(this))) {
	Reg<SObj>();

	// primitive
	Reg<Engine::Sphere>();
	Reg<Engine::Plane>();
	Reg<TriMesh>();

	// bsdf
	Reg<BSDF_Diffuse>();
	Reg<BSDF_Emission>();
	Reg<BSDF_Glass>();
	Reg<BSDF_Mirror>();
	Reg<BSDF_CookTorrance>();
	Reg<BSDF_MetalWorkflow>();
	Reg<BSDF_FrostedGlass>();

	// light
	Reg<AreaLight>();
}

void Impl_Raster::Init() {
	pldmGenerator->Init();

	//------------- light UBO
	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 400, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	InitVAOs();

	InitShaders();
}

void Impl_Raster::InitVAOs() {
	InitVAO_Sphere();
	InitVAO_Plane();
}

void Impl_Raster::InitVAO_Sphere() {
	Basic::Sphere sphere(50);
	vector<VAO::VBO_DataPatch> P3_Sphere_Vec_VBO_Data_Patch = {
		{sphere.GetPosArr(), sphere.GetPosArrSize(), 3},
		{sphere.GetNormalArr(), sphere.GetNormalArrSize(), 3},
		{sphere.GetTexCoordsArr(), sphere.GetTexCoordsArrSize(), 2},
		{sphere.GetTangentArr(), sphere.GetTangentArrSize(), 3},
	};
	VAO_P3N3T2T3_Sphere = VAO(P3_Sphere_Vec_VBO_Data_Patch, sphere.GetIndexArr(), sphere.GetIndexArrSize());
}

void Impl_Raster::InitVAO_Plane() {
	auto plane = TriMesh::GenPlane();
	vector<VAO::VBO_DataPatch> P3_Plane_Vec_VBO_Data_Patch = {
		{&(plane->GetPositions()[0][0]), static_cast<uint>(plane->GetPositions().size() * 3 * sizeof(float)), 3},
		{&(plane->GetNormals()[0][0]), static_cast<uint>(plane->GetNormals().size() * 3 * sizeof(float)), 3},
		{&(plane->GetTexcoords()[0][0]), static_cast<uint>(plane->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{&(plane->GetTangents()[0][0]), static_cast<uint>(plane->GetTangents().size() * 3 * sizeof(float)), 3},
	};
	VAO_P3N3T2T3_Plane = VAO(P3_Plane_Vec_VBO_Data_Patch, plane->GetIndice().data(), static_cast<uint>(plane->GetIndice().size() * sizeof(uint)));
}

void Impl_Raster::InitShaders() {
	InitShaderBasic();
	InitShaderDiffuse();
	InitShaderMetalWorkflow();
	InitShaderFrostedGlass();
}

void Impl_Raster::InitShaderBasic() {
	shader_basic = Shader(rootPath + str_Basic_P3_vs, rootPath + str_Basic_fs);
	shader_basic.UniformBlockBind("Camera", 0);
}

void Impl_Raster::InitShaderDiffuse() {
	shader_diffuse = Shader(rootPath + str_Basic_P3N3T2_vs, rootPath + "data/shaders/Engine/BSDF_Diffuse.fs");
	shader_diffuse.UniformBlockBind("Camera", 0);
	shader_diffuse.UniformBlockBind("PointLights", 1);

	shader_diffuse.SetInt("bsdf.albedoTexture", 0);
	for (int i = 0; i < maxPointLights; i++)
		shader_diffuse.SetInt("pointLightDepthMap" + to_string(i), 1 + i);

	shader_diffuse.SetFloat("lightFar", pldmGenerator->GetLightFar());
}

void Impl_Raster::InitShaderMetalWorkflow() {
	shader_metalWorkflow = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/BSDF_MetalWorkflow.fs");
	shader_metalWorkflow.UniformBlockBind("Camera", 0);
	shader_metalWorkflow.UniformBlockBind("PointLights", 1);

	shader_metalWorkflow.SetInt("bsdf.albedoTexture", 0);
	shader_metalWorkflow.SetInt("bsdf.metallicTexture", 1);
	shader_metalWorkflow.SetInt("bsdf.roughnessTexture", 2);
	shader_metalWorkflow.SetInt("bsdf.aoTexture", 3);
	shader_metalWorkflow.SetInt("bsdf.normalTexture", 4);
	for (int i = 0; i < maxPointLights; i++)
		shader_metalWorkflow.SetInt("pointLightDepthMap" + to_string(i), 5 + i);

	shader_metalWorkflow.SetFloat("lightFar", pldmGenerator->GetLightFar());
}

void Impl_Raster::InitShaderFrostedGlass() {
	shader_frostedGlass = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/BSDF_FrostedGlass.fs");
	shader_frostedGlass.UniformBlockBind("Camera", 0);
	shader_frostedGlass.UniformBlockBind("PointLights", 1);

	shader_frostedGlass.SetInt("bsdf.colorTexture", 0);
	shader_frostedGlass.SetInt("bsdf.roughnessTexture", 1);
	shader_frostedGlass.SetInt("bsdf.aoTexture", 2);
	shader_frostedGlass.SetInt("bsdf.normalTexture", 3);
	for (int i = 0; i < maxPointLights; i++)
		shader_frostedGlass.SetInt("pointLightDepthMap" + to_string(i), 4 + i);

	shader_frostedGlass.SetFloat("lightFar", pldmGenerator->GetLightFar());
}

void Impl_Raster::Draw() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene->Accept(pldmGenerator);

	modelVec.clear();
	modelVec.push_back(mat4(1.0f));
	scene->GetRoot()->Accept(This());

	UpdateLights();
}

void Impl_Raster::Visit(SObj::Ptr sobj) {
	auto geometry = sobj->GetComponent<Geometry>();
	auto children = sobj->GetChildren();
	// 这种情况下不需要 transform
	if (geometry == nullptr && children.size() == 0)
		return;

	auto transform = sobj->GetComponent<Transform>();
	if (transform != nullptr)
		modelVec.push_back(modelVec.back() * transform->GetMat());

	if (geometry && geometry->GetPrimitive()) {
		auto primitive = geometry->GetPrimitive();

		// Set Shader
		auto material = sobj->GetComponent<Material>();
		if (material && material->GetMat())
			material->GetMat()->Accept(This());
		else {
			auto lightComponent = sobj->GetComponent<Light>();
			if (lightComponent && lightComponent->GetLight())
				lightComponent->GetLight()->Accept(This());
			else {
				curShader = shader_basic;
				shader_basic.SetVec3f("color", vec3(1, 0, 1));
			}
		}

		primitive->Accept(This());
	}

	for (auto child : children)
		child->Accept(This());

	if (transform != nullptr)
		modelVec.pop_back();
}

void Impl_Raster::Visit(Engine::Sphere::Ptr sphere) {
	mat4 model = modelVec.back();
	model = translate(model, sphere->center);
	model = scale(model, vec3(sphere->r));

	curShader.SetMat4f("model", model);

	VAO_P3N3T2T3_Sphere.Draw(curShader);
}

void Impl_Raster::Visit(Engine::Plane::Ptr plane) {
	curShader.SetMat4f("model", modelVec.back());
	VAO_P3N3T2T3_Plane.Draw(curShader);
}

void Impl_Raster::Visit(TriMesh::Ptr mesh) {
	curShader.SetMat4f("model", modelVec.back());

	auto meshVAO = GetMeshVAO(mesh);

	meshVAO.Draw(curShader);
}

void Impl_Raster::Visit(BSDF_Diffuse::Ptr bsdf) {
	curShader = shader_diffuse;
	string strBSDF = "bsdf.";
	shader_diffuse.SetVec3f(strBSDF + "albedoColor", bsdf->albedoColor);
	if (bsdf->albedoTexture && bsdf->albedoTexture->IsValid()) {
		shader_diffuse.SetBool(strBSDF + "haveAlbedoTexture", true);
		GetTex(bsdf->albedoTexture).Use(0);
	}
	else
		shader_diffuse.SetBool(strBSDF + "haveAlbedoTexture", false);

	SetPointLightDepthMap(shader_diffuse, 1);
}

void Impl_Raster::Visit(BSDF_Glass::Ptr bsdf) {
	curShader = shader_basic;
	shader_basic.SetVec3f("color", bsdf->transmittance);
}

void Impl_Raster::Visit(BSDF_Mirror::Ptr bsdf) {
	curShader = shader_basic;
	shader_basic.SetVec3f("color", bsdf->reflectance);
}

void Impl_Raster::Visit(BSDF_Emission::Ptr bsdf) {
	curShader = shader_basic;
	shader_basic.SetVec3f("color", bsdf->GetEmission());
}

void Impl_Raster::Visit(BSDF_CookTorrance::Ptr bsdf) {
	curShader = shader_basic;
	shader_basic.SetVec3f("color", bsdf->refletance);
}

void Impl_Raster::Visit(BSDF_MetalWorkflow::Ptr bsdf) {
	curShader = shader_metalWorkflow;

	string strBSDF = "bsdf.";
	shader_metalWorkflow.SetVec3f(strBSDF + "albedoColor", bsdf->albedoColor);
	shader_metalWorkflow.SetFloat(strBSDF + "metallicFactor", bsdf->metallicFactor);
	shader_metalWorkflow.SetFloat(strBSDF + "roughnessFactor", bsdf->roughnessFactor);

	const int texNum = 5;
	Image::CPtr imgs[texNum] = { bsdf->albedoTexture, bsdf->metallicTexture, bsdf->roughnessTexture, bsdf->aoTexture, bsdf->normalTexture };
	string names[texNum] = { "Albedo", "Metallic", "Roughness", "AO", "Normal" };

	for (int i = 0; i < texNum; i++) {
		string wholeName = strBSDF + "have" + names[i] + "Texture";
		if (imgs[i] && imgs[i]->IsValid()) {
			shader_metalWorkflow.SetBool(wholeName, true);
			GetTex(imgs[i]).Use(i);
		}
		else
			shader_metalWorkflow.SetBool(wholeName, false);
	}

	SetPointLightDepthMap(shader_metalWorkflow, texNum);
}

void Impl_Raster::Visit(BSDF_FrostedGlass::Ptr bsdf) {
	curShader = shader_frostedGlass;

	string strBSDF = "bsdf.";
	shader_frostedGlass.SetVec3f(strBSDF + "colorFactor", bsdf->colorFactor);
	shader_frostedGlass.SetFloat(strBSDF + "roughnessFactor", bsdf->roughnessFactor);

	const int texNum = 4;
	Image::CPtr imgs[texNum] = { bsdf->colorTexture, bsdf->roughnessTexture, bsdf->aoTexture, bsdf->normalTexture };
	string names[texNum] = { "Color", "Roughness", "AO", "Normal" };

	for (int i = 0; i < texNum; i++) {
		string wholeName = strBSDF + "have" + names[i] + "Texture";
		if (imgs[i] && imgs[i]->IsValid()) {
			shader_frostedGlass.SetBool(wholeName, true);
			GetTex(imgs[i]).Use(i);
		}
		else
			shader_frostedGlass.SetBool(wholeName, false);
	}

	shader_frostedGlass.SetFloat(strBSDF + "ior", bsdf->ior);

	SetPointLightDepthMap(shader_frostedGlass, texNum);
}

void Impl_Raster::Visit(AreaLight::Ptr areaLight) {
	curShader = shader_basic;
	shader_basic.SetVec3f("color", areaLight->color);
}

Texture Impl_Raster::GetTex(Image::CPtr img) {
	auto target = img2tex.find(img);
	if (target != img2tex.end())
		return target->second;

	auto tex = Texture(img);
	img2tex[img] = tex;
	return tex;
}

void Impl_Raster::UpdateLights() const {
	int lightIdx = 0;
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	for (auto lightComponent : scene->GetLights()) {
		auto pointLight = PointLight::Ptr::Cast(lightComponent->GetLight());
		if (!pointLight)
			continue;

		vec3 position = lightComponent->GetSObj()->GetLocalToWorldMatrix()*vec4(0, 0, 0, 1);

		int base = 16 + 48 * lightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, glm::value_ptr(position));
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, glm::value_ptr(pointLight->intensity * pointLight->color));
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28, 4, &pointLight->linear);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 4, &pointLight->quadratic);

		lightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &lightIdx);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

VAO Impl_Raster::GetMeshVAO(TriMesh::CPtr mesh) {
	auto target = meshVAOs.find(mesh);
	if (target != meshVAOs.end())
		return target->second;

	vector<VAO::VBO_DataPatch> P3_Mesh_Vec_VBO_Data_Patch = {
		{&(mesh->GetPositions()[0][0]), static_cast<uint>(mesh->GetPositions().size() * 3 * sizeof(float)), 3},
		{&(mesh->GetNormals()[0][0]), static_cast<uint>(mesh->GetNormals().size() * 3 * sizeof(float)), 3},
		{&(mesh->GetTexcoords()[0][0]), static_cast<uint>(mesh->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{&(mesh->GetTangents()[0][0]), static_cast<uint>(mesh->GetTangents().size() * 3 * sizeof(float)), 3},
	};

	VAO VAO_P3N3T2T3_Mesh(P3_Mesh_Vec_VBO_Data_Patch, mesh->GetIndice().data(), static_cast<uint>(mesh->GetIndice().size() * sizeof(uint)));
	// 用 TriMesh::Ptr 做 ID
	meshVAOs[mesh] = VAO_P3N3T2T3_Mesh;

	return VAO_P3N3T2T3_Mesh;
}

void Impl_Raster::SetPointLightDepthMap(Shader shader, int base) {
	int lightIdx = 0;
	for (auto lightComponent : scene->GetLights()) {
		if (!PointLight::Ptr::Cast(lightComponent->GetLight()))
			continue;

		pldmGenerator->GetDepthCubeMap(lightComponent).Use(base + lightIdx);

		lightIdx++;
	}
}
