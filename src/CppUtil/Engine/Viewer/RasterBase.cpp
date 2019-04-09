#include <CppUtil/Engine/RasterBase.h>

#include "PLDM_Generator.h"

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
using namespace CppUtil::QT;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace std;

const int RasterBase::maxPointLights = 8;

RasterBase::RasterBase(Ptr<Scene> scene)
	: scene(scene), pldmGenerator(PLDM_Generator::New(this)) {
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

	UpdateLights();

	modelVec.clear();
	modelVec.push_back(Transform(1.f));
	scene->GetRoot()->Accept(This());
}

void RasterBase::OGL_Init() {
	glGenBuffers(1, &lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 400, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	InitVAOs();

	pldmGenerator->OGL_Init();
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
	VAO_P3N3T2T3_Sphere.Draw(curShader);
}

void RasterBase::Visit(Ptr<Engine::Plane> plane) {
	curShader.SetMat4f("model", modelVec.back());
	VAO_P3N3T2T3_Plane.Draw(curShader);
}

void RasterBase::Visit(Ptr<TriMesh> mesh) {
	curShader.SetMat4f("model", modelVec.back());
	GetMeshVAO(mesh).Draw(curShader);
}

const Texture RasterBase::GetTex(PtrC<Image> img) {
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
	for (auto cmptLight : scene->GetCmptLights()) {
		auto pointLight = CastTo<PointLight>(cmptLight->light);
		if (!pointLight)
			continue;

		Point3 position = cmptLight->GetSObj()->GetWorldPos();

		int base = 16 + 48 * lightIdx;
		glBufferSubData(GL_UNIFORM_BUFFER, base, 12, position.Data());
		auto lightL = pointLight->intensity * pointLight->color;
		glBufferSubData(GL_UNIFORM_BUFFER, base + 16, 12, lightL.Data());
		glBufferSubData(GL_UNIFORM_BUFFER, base + 28, 4, &pointLight->linear);
		glBufferSubData(GL_UNIFORM_BUFFER, base + 32, 4, &pointLight->quadratic);

		lightIdx++;
	}
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &lightIdx);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

const VAO RasterBase::GetMeshVAO(PtrC<TriMesh> mesh) {
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
	for (auto cmptLight : scene->GetCmptLights()) {
		if (!CastTo<PointLight>(cmptLight->light))
			continue;

		pldmGenerator->GetDepthCubeMap(cmptLight).Use(base + lightIdx);

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
