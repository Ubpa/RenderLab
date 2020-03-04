#include <Engine/Viewer.h>

#include <Engine/DirectIllumRaster.h>
#include <Engine/DeferredRaster.h>
#include <Engine/ForwardNPR.h>
#include <Engine/WireframeRaster.h>

#include <Engine/Roamer.h>
#include "Picker.h"

#include <Qt/RawAPI_OGLW.h>

#include <Basic/LambdaOp.h>

using namespace CppUtil::Engine;
using namespace CppUtil::QT;
using namespace CppUtil::Basic;

Viewer::Viewer(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, RasterType rasterType)
	: pOGLW(pOGLW), scene(scene), roamer(Roamer::New(pOGLW)), picker(Picker::New(this)) {
	SetRaster(rasterType);

	pOGLW->AddInitOp(LambdaOp_New([this]() {
		roamer->Init();
		picker->Init();
	}, false));

	pOGLW->AddPaintOp(LambdaOp_New([this]() {
		raster->Draw();
	}, true));

	pOGLW->AddResizeOp(LambdaOp_New([this]() {
		auto pOGLW = this->GetOGLW();

		int w = pOGLW->w;
		int h = pOGLW->h;

		roamer->SetWH(w, h);
		raster->Resize();
	}, true));
}

void Viewer::SetLock(bool isLock) {
	roamer->SetLock(isLock);
}

void Viewer::SetRaster(RasterType rasterType) {
	switch (rasterType)
	{
	case RasterType::DirectIllum:
		raster = DirectIllumRaster::New(pOGLW, scene, roamer->GetCamera());
		break;
	case RasterType::DeferredPipeline:
		raster = DeferredRaster::New(pOGLW, scene, roamer->GetCamera());
		break;
	case RasterType::ForwardNPR:
		raster = ForwardNPR::New(pOGLW, scene, roamer->GetCamera());
		break;
	case RasterType::WireframeRaster:
		raster = WireframeRaster::New(pOGLW, scene, roamer->GetCamera());
		break;
	default:
		printf("ERROR::Viewer::Viewer:\n"
			"\t""not support RasterType(%s)\n", rasterType._to_string());
		break;
	}

	pOGLW->AddInitOp(LambdaOp_New([this]() {
		raster->Init();
	}, false));
}
