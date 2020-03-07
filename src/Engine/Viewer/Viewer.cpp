#include <Engine/Viewer/Viewer.h>

#include <Engine/Viewer/DirectIllumRaster.h>
#include <Engine/Viewer/DeferredRaster.h>
#include <Engine/Viewer/ForwardNPR.h>
#include <Engine/Viewer/WireframeRaster.h>

#include <Engine/Viewer/Roamer.h>
#include "Picker.h"

#include <Qt/RawAPI_OGLW.h>

#include <Basic/Op/LambdaOp.h>

using namespace Ubpa;

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
			"\t""not support RasterType\n");
		break;
	}

	pOGLW->AddInitOp(LambdaOp_New([this]() {
		raster->Init();
	}, false));
}
