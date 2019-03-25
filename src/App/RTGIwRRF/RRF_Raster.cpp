#include "RRF_Raster.h"

#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Material.h>
#include <CppUtil/Engine/Geometry.h>
#include <CppUtil/Engine/BSDF_FrostedGlass.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/EventManager.h>
#include <CppUtil/Basic/LambdaOp.h>

#include <CppUtil/OpenGL/CommonDefine.h>

#include <ROOT_PATH.h>

using namespace App;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace Define;
using namespace std;

void RRF_Raster::Init() {
	RasterBase::Init();

	scene->GenID();

	InitListeners();
	InitShaders();

	glViewport(0, 0, 512, 512);
}

void RRF_Raster::InitListeners() {
	EventMngr::GetInstance().Reg(static_cast<size_t>(Qt::Key_1), EventMngr::ENUM_EVENT_TYPE::KB_PRESS, 
		ToPtr(new LambdaOp([=]() {
		for (auto & pair : id2shader) {
			pair.second.SetInt("mode", MODE::DIRECT);
		}
	})));
	EventMngr::GetInstance().Reg(static_cast<size_t>(Qt::Key_2), EventMngr::ENUM_EVENT_TYPE::KB_PRESS,
		ToPtr(new LambdaOp([=]() {
		for (auto & pair : id2shader) {
			pair.second.SetInt("mode", MODE::INDIRECT);
		}
	})));
	EventMngr::GetInstance().Reg(static_cast<size_t>(Qt::Key_3), EventMngr::ENUM_EVENT_TYPE::KB_PRESS,
		ToPtr(new LambdaOp([=]() {
		for (auto & pair : id2shader) {
			pair.second.SetInt("mode", MODE::GLOBAL);
		}
	})));
}

void RRF_Raster::InitShaders() {
	vector<int> IDs;
	auto visitor = ToPtr(new EleVisitor);
	visitor->Reg<SObj>([&](SObj::Ptr sobj) {
		auto material = sobj->GetComponent<Material>();
		auto geometry = sobj->GetComponent<Geometry>();
		if (!material || !geometry)
			return;

		auto bsdf = BSDF_FrostedGlass::Ptr::Cast(material->GetMat());
		if (bsdf == nullptr)
			return;

		IDs.push_back(scene->GetID(sobj));
	});
	scene->GetRoot()->TraverseAccept(visitor);

	for (auto ID : IDs)
		InitShader(ID);
}

void RRF_Raster::InitShader(int ID) {
	printf("init shader of ID %d ...\n", ID);

	string fsPath = ROOT_PATH + "data/shaders/App/RTGIwRRF/" + to_string(ID) + "_modelKDTree.fs";
	auto shader = Shader(ROOT_PATH + str_Basic_P3N3T2T3_vs, fsPath);
	if (!shader.IsValid())
		return;

	id2shader[ID] = shader;
	BindBlock(shader);

	shader.SetInt("bsdf.colorTexture", 0);
	shader.SetInt("bsdf.roughnessTexture", 1);
	shader.SetInt("bsdf.aoTexture", 2);
	shader.SetInt("bsdf.normalTexture", 3);

	shader.SetInt("mode", MODE::GLOBAL);

	SetShaderForShadow(shader, 4);
	printf("init shader of ID %d success\n", ID);
}

void RRF_Raster::Draw() {
	RasterBase::Draw();
}

void RRF_Raster::Visit(SObj::Ptr sobj) {
	curID = scene->GetID(sobj);

	RasterBase::Visit(sobj);
}

void RRF_Raster::Visit(BSDF_FrostedGlass::Ptr bsdf) {
	auto target = id2shader.find(curID);
	if (target == id2shader.end()) {
		printf("ERROR: shader of ID[%d] not found\n", curID);
		return;
	}

	auto & shader = target->second;
	SetCurShader(shader);

	string strBSDF = "bsdf.";
	shader.SetVec3f(strBSDF + "colorFactor", bsdf->colorFactor);
	shader.SetFloat(strBSDF + "roughnessFactor", bsdf->roughnessFactor);

	const int texNum = 4;
	Image::CPtr imgs[texNum] = { bsdf->colorTexture, bsdf->roughnessTexture, bsdf->aoTexture, bsdf->normalTexture };
	string names[texNum] = { "Color", "Roughness", "AO", "Normal" };

	for (int i = 0; i < texNum; i++) {
		string wholeName = strBSDF + "have" + names[i] + "Texture";
		if (imgs[i] && imgs[i]->IsValid()) {
			shader.SetBool(wholeName, true);
			GetTex(imgs[i]).Use(i);
		}
		else
			shader.SetBool(wholeName, false);
	}

	shader.SetFloat(strBSDF + "ior", bsdf->ior);

	SetPointLightDepthMap(shader, texNum);
}
