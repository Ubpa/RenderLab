#include <CppUtil/Engine/Viewer.h>

#include "Raster.h"
#include "Roamer.h"
#include "Picker.h"

#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Basic/LambdaOp.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Qt;
using namespace CppUtil::Basic;

Viewer::Viewer(RawAPI_OGLW * pOGLW, CppUtil::Basic::Ptr<Scene> scene)
	: pOGLW(pOGLW), scene(scene), raster(ToPtr(new Raster(scene))), roamer(ToPtr(new Roamer(pOGLW))), picker(ToPtr(new Picker(this))) {
	pOGLW->SetInitOp(ToPtr(new LambdaOp([this]() {
		this->GetRoamer()->Init();
		this->GetRaster()->Init();
		this->GetPicker()->Init();
	})));

	pOGLW->SetPaintOp(ToPtr(new LambdaOp([this]() {
		this->GetRaster()->Draw();
	})));

	pOGLW->SetResizeOp(ToPtr(new LambdaOp([this]() {
		auto pOGLW = this->GetOGLW();

		int w = pOGLW->w;
		int h = pOGLW->h;

		this->GetRoamer()->SetWH(w, h);
	})));
}

void Viewer::SetLock(bool isLock) {
	roamer->lock = isLock;
}
