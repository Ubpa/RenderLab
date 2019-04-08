#include <CppUtil/Engine/Viewer.h>

#include "DirectIllumRaster.h"
#include <CppUtil/Engine/Roamer.h>
#include "Picker.h"

#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Basic/LambdaOp.h>

using namespace CppUtil::Engine;
using namespace CppUtil::QT;
using namespace CppUtil::Basic;

Viewer::Viewer(RawAPI_OGLW * pOGLW, Ptr<Scene> scene)
	: pOGLW(pOGLW), scene(scene), raster(DirectIllumRaster::New(scene)), roamer(Roamer::New(pOGLW)), picker(Picker::New(this)) {
	pOGLW->SetInitOp(LambdaOp_New([this]() {
		this->GetRoamer()->OGL_Init();
		this->GetRaster()->OGL_Init();
		this->GetPicker()->OGL_Init();
	}));

	pOGLW->SetPaintOp(LambdaOp_New([this]() {
		this->GetRaster()->Draw();
	}));

	pOGLW->SetResizeOp(LambdaOp_New([this]() {
		auto pOGLW = this->GetOGLW();

		int w = pOGLW->w;
		int h = pOGLW->h;

		this->GetRoamer()->SetWH(w, h);
	}));
}

void Viewer::SetLock(bool isLock) {
	roamer->SetLock(isLock);
}
