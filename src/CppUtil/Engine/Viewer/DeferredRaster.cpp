#include <CppUtil/Engine/DeferredRaster.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/CmptTransform.h>
#include <CppUtil/Engine/CmptMaterial.h>

#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>
#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Engine/BSDF_MetalWorkflow.h>

#include <CppUtil/OpenGL/CommonDefine.h>

#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

using namespace Define;
using namespace std;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::QT;

const string rootPath = ROOT_PATH;

DeferredRaster::DeferredRaster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
	: Raster(pOGLW,scene,camera)
{
	RegMemberFunc<SObj>(&DeferredRaster::Visit);

	// primitive
	RegMemberFunc<Sphere>(&DeferredRaster::Visit);
	RegMemberFunc<Plane>(&DeferredRaster::Visit);
	RegMemberFunc<TriMesh>(&DeferredRaster::Visit);

	// metal
	RegMemberFunc<BSDF_MetalWorkflow>(&DeferredRaster::Visit);
}

void DeferredRaster::Init() {
	Raster::Init();

	metalShader = Shader(rootPath + str_Basic_P3N3T2T3_vs, rootPath + "data/shaders/Engine/DeferredPipline/Metal_GBuffer.fs");

	metalShader.SetInt("metal.albedoTexture", 0);
	metalShader.SetInt("metal.metallicTexture", 1);
	metalShader.SetInt("metal.roughnessTexture", 2);
	metalShader.SetInt("metal.aoTexture", 3);
	metalShader.SetInt("metal.normalTexture", 4);

	BindUBO(metalShader);

	gbufferFBO = FBO(pOGLW->width(), pOGLW->height(), { 3,3,3 });
}

void DeferredRaster::Resize() {
	gbufferFBO = FBO(pOGLW->w, pOGLW->h, { 3,3,3 });
}

void DeferredRaster::Draw() {
	glEnable(GL_DEPTH_TEST);

	Pass_GBuffer();
}

void DeferredRaster::Pass_GBuffer() {
	gbufferFBO.Use();
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	modelVec.clear();
	modelVec.push_back(Transform(1.f));
	scene->GetRoot()->Accept(This());
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
		if (!cmptMaterial)
			break;

		auto metal = CastTo<BSDF_MetalWorkflow>(cmptMaterial->material);
		if (!metal)
			break;

		metal->Accept(This());
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
}

void DeferredRaster::Visit(Ptr<Sphere> sphere) {
	metalShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Sphere).Draw(metalShader);
}

void DeferredRaster::Visit(Ptr<Plane> plane) {
	metalShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(ShapeType::Plane).Draw(metalShader);
}

void DeferredRaster::Visit(Ptr<TriMesh> mesh) {
	metalShader.SetMat4f("model", modelVec.back());
	pOGLW->GetVAO(mesh).Draw(metalShader);
}
