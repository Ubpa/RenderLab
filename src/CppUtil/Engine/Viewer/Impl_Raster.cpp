#include "Impl_Raster.h"

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/Geometry.h>
#include <CppUtil/Engine/Material.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/AllBSDFs.h>

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

Impl_Raster::Impl_Raster(RawAPI_OGLW * pOGLW, Scene::Ptr scene)
	: pOGLW(pOGLW), scene(scene) {
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
}

void Impl_Raster::Init() {
	//------------ 模型 . P3_Sphere
	Basic::Sphere sphere(50);
	vector<VAO::VBO_DataPatch> P3_Sphere_Vec_VBO_Data_Patch = {
		{sphere.GetPosArr(), sphere.GetPosArrSize(), 3},
	};
	VAO_P3_Sphere = VAO(P3_Sphere_Vec_VBO_Data_Patch, sphere.GetIndexArr(), sphere.GetIndexArrSize());


	//------------ 模型 . P3_Plane
	Basic::Plane plane;
	vector<VAO::VBO_DataPatch> P3_Plane_Vec_VBO_Data_Patch = {
		{plane.GetPosArr(), plane.GetPosArrSize(), 3},
	};
	VAO_P3_Plane = VAO(P3_Plane_Vec_VBO_Data_Patch, plane.GetIndexArr(), plane.GetIndexArrSize());

	//------------ 着色器 . basic
	shader_basic = Shader(rootPath + str_Basic_P3_vs, rootPath + str_Basic_fs);
	shader_basic.UniformBlockBind("CameraMatrixs", 0);

	auto geos = scene->GetRootSObj()->GetComponentsInChildren<Geometry>();
	for (auto geo : geos) {
		TriMesh::Ptr mesh = geo->GetPrimitive();
		if (mesh == nullptr)
			continue;

		vector<VAO::VBO_DataPatch> P3_Mesh_Vec_VBO_Data_Patch = {
			{ &(mesh->GetPositions()[0][0]), mesh->GetPositions().size() * 3 * sizeof(float), 3 },
		};

		VAO VAO_P3_Mesh(P3_Mesh_Vec_VBO_Data_Patch, mesh->GetIndice().data(), mesh->GetIndice().size() * sizeof(size_t));
		// 用 TriMesh::Ptr 做 ID
		meshVAOs[mesh] = VAO_P3_Mesh;
	}
}

void Impl_Raster::Draw() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelVec.clear();
	modelVec.push_back(mat4(1.0f));
	scene->GetRootSObj()->Accept(This());
}

void Impl_Raster::Draw(SObj::Ptr sobj) {
	auto geometry = sobj->GetComponent<Geometry>();
	auto children = sobj->GetChildren();
	// 这种情况下不需要 transform
	if (geometry == nullptr && children.size() == 0)
		return;

	auto transform = sobj->GetComponent<Transform>();
	if (transform != nullptr)
		modelVec.push_back(modelVec.back() * transform->GetMat());

	if (geometry) {
		auto primitive = geometry->GetPrimitive();
		if (primitive) {
			auto material = sobj->GetComponent<Material>();
			if (material)
				material->GetMat()->Accept(This());
			primitive->Accept(This());
		}
	}

	for (auto child : children)
		child->Accept(This());

	if (transform != nullptr)
		modelVec.pop_back();
}

void Impl_Raster::Draw(Engine::Sphere::Ptr sphere) {
	mat4 model = modelVec.back();
	model = translate(model, sphere->GetCenter());
	model = scale(model, vec3(sphere->GetR()));

	shader_basic.SetMat4f("model", model);
	
	VAO_P3_Sphere.Draw(shader_basic);
}

void Impl_Raster::Draw(Engine::Plane::Ptr plane) {
	shader_basic.SetMat4f("model", modelVec.back());
	VAO_P3_Plane.Draw(shader_basic);
}

void Impl_Raster::Draw(TriMesh::Ptr mesh) {
	shader_basic.SetMat4f("model", modelVec.back());

	auto target = meshVAOs.find(mesh);
	if (target == meshVAOs.end()) {
		vector<VAO::VBO_DataPatch> P3_Mesh_Vec_VBO_Data_Patch = {
				{ &(mesh->GetPositions()[0][0]), mesh->GetPositions().size() * 3 * sizeof(float), 3 },
		};

		VAO VAO_P3_Mesh(P3_Mesh_Vec_VBO_Data_Patch, mesh->GetIndice().data(), mesh->GetIndice().size() * sizeof(size_t));
		// 用 TriMesh::Ptr 做 ID
		meshVAOs[mesh] = VAO_P3_Mesh;
	}

	meshVAOs[mesh].Draw(shader_basic);
}

void Impl_Raster::Draw(BSDF_Diffuse::Ptr bsdf) {
	shader_basic.SetVec3f("color", bsdf->GetAlbedo());
}

void Impl_Raster::Draw(BSDF_Glass::Ptr bsdf) {
	shader_basic.SetVec3f("color", bsdf->GetReflectance());
}

void Impl_Raster::Draw(BSDF_Mirror::Ptr bsdf) {
	shader_basic.SetVec3f("color", bsdf->GetReflectance());
}

void Impl_Raster::Draw(BSDF_Emission::Ptr bsdf) {
	shader_basic.SetVec3f("color", bsdf->GetEmission());
}
