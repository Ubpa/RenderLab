#include <CppUtil/Engine/RasterBase.h>

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

using namespace CppUtil::Engine;
using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace glm;
using namespace std;

const int RasterBase::maxPointLights = 8;

RasterBase::RasterBase(Scene::Ptr scene)
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
}

void RasterBase::Draw() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene->Accept(pldmGenerator);

	UpdateLights();

	modelVec.clear();
	modelVec.push_back(mat4(1.0f));
	scene->GetRoot()->Accept(This());
}

void RasterBase::Init() {
	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 400, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	InitVAOs();

	pldmGenerator->Init();
}

void RasterBase::InitVAOs() {
	InitVAO_Sphere();
	InitVAO_Plane();
}

void RasterBase::InitVAO_Sphere() {
	Basic::Sphere sphere(50);
	vector<VAO::VBO_DataPatch> P3_Sphere_Vec_VBO_Data_Patch = {
		{sphere.GetPosArr(), sphere.GetPosArrSize(), 3},
		{sphere.GetNormalArr(), sphere.GetNormalArrSize(), 3},
		{sphere.GetTexCoordsArr(), sphere.GetTexCoordsArrSize(), 2},
		{sphere.GetTangentArr(), sphere.GetTangentArrSize(), 3},
	};
	VAO_P3N3T2T3_Sphere = VAO(P3_Sphere_Vec_VBO_Data_Patch, sphere.GetIndexArr(), sphere.GetIndexArrSize());
}

void RasterBase::InitVAO_Plane() {
	auto plane = TriMesh::GenPlane();
	vector<VAO::VBO_DataPatch> P3_Plane_Vec_VBO_Data_Patch = {
		{&(plane->GetPositions()[0][0]), static_cast<uint>(plane->GetPositions().size() * 3 * sizeof(float)), 3},
		{&(plane->GetNormals()[0][0]), static_cast<uint>(plane->GetNormals().size() * 3 * sizeof(float)), 3},
		{&(plane->GetTexcoords()[0][0]), static_cast<uint>(plane->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{&(plane->GetTangents()[0][0]), static_cast<uint>(plane->GetTangents().size() * 3 * sizeof(float)), 3},
	};
	VAO_P3N3T2T3_Plane = VAO(P3_Plane_Vec_VBO_Data_Patch, plane->GetIndice().data(), static_cast<uint>(plane->GetIndice().size() * sizeof(uint)));
}

void RasterBase::Visit(SObj::Ptr sobj) {
	auto geometry = sobj->GetComponent<Geometry>();
	auto material = sobj->GetComponent<Material>();
	auto children = sobj->GetChildren();

	// 这种情况下不需要 transform
	if ((!geometry || !geometry->GetPrimitive() || !material || !material->GetMat())
		&& children.size() == 0)
		return;

	auto transform = sobj->GetComponent<Transform>();
	if (transform != nullptr)
		modelVec.push_back(modelVec.back() * transform->GetMat());

	if (geometry && geometry->GetPrimitive() && material && material->GetMat()) {
		material->GetMat()->Accept(This());
		geometry->GetPrimitive()->Accept(This());
	}

	for (auto child : children)
		child->Accept(This());

	if (transform != nullptr)
		modelVec.pop_back();
}

void RasterBase::Visit(Engine::Sphere::Ptr sphere) {
	mat4 model = modelVec.back();
	model = translate(model, sphere->center);
	model = scale(model, vec3(sphere->r));

	curShader.SetMat4f("model", model);

	VAO_P3N3T2T3_Sphere.Draw(curShader);
}

void RasterBase::Visit(Engine::Plane::Ptr plane) {
	curShader.SetMat4f("model", modelVec.back());
	VAO_P3N3T2T3_Plane.Draw(curShader);
}

void RasterBase::Visit(TriMesh::Ptr mesh) {
	curShader.SetMat4f("model", modelVec.back());

	auto meshVAO = GetMeshVAO(mesh);

	meshVAO.Draw(curShader);
}

Texture RasterBase::GetTex(Image::CPtr img) {
	auto target = img2tex.find(img);
	if (target != img2tex.end())
		return target->second;

	auto tex = Texture(img);
	img2tex[img] = tex;
	return tex;
}

void RasterBase::UpdateLights() const {
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

VAO RasterBase::GetMeshVAO(TriMesh::CPtr mesh) {
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

void RasterBase::SetPointLightDepthMap(const Shader & shader, int base) {
	int lightIdx = 0;
	for (auto lightComponent : scene->GetLights()) {
		if (!PointLight::Ptr::Cast(lightComponent->GetLight()))
			continue;

		pldmGenerator->GetDepthCubeMap(lightComponent).Use(base + lightIdx);

		lightIdx++;
	}
}

void RasterBase::SetShaderForShadow(const OpenGL::Shader & shader, int base) const {
	for (int i = 0; i < maxPointLights; i++)
		shader.SetInt("pointLightDepthMap" + to_string(i), base + i);

	shader.SetFloat("lightFar", pldmGenerator->GetLightFar());
}

void RasterBase::BindBlock(const OpenGL::Shader & shader) const {
	shader.UniformBlockBind("Camera", 0);
	shader.UniformBlockBind("PointLights", 1);
}
