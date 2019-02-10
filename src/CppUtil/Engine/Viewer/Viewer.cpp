#include <CppUtil/Engine/Viewer.h>

#include <CppUtil/Engine/Raster.h>
#include <CppUtil/Engine/Roamer.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Basic/LambdaOp.h>

#include <qdebug.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;

Viewer::Viewer(RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene)
	: pOGLW(pOGLW), scene(scene), raster(ToPtr(new Raster(scene))), roamer(ToPtr(new Roamer(pOGLW))) {
	pOGLW->SetInitOp(ToPtr(new LambdaOp([this]() {
		this->GetRoamer()->Init();
		this->GetRaster()->Init();
	})));

	pOGLW->SetPaintOp(ToPtr(new LambdaOp([this]() {
		this->GetRaster()->Draw();
	})));

	pOGLW->SetResizeOp(ToPtr(new LambdaOp([this]() {
		auto pOGLW = this->GetOGLW();

		int * w;
		int * h;
		pOGLW->GetP(RawAPI_OGLW::str_w, w);
		pOGLW->GetP(RawAPI_OGLW::str_h, h);
		if (!w || !h) {
			qDebug() << "get w or h fail\n";
			return;
		}
		this->GetRoamer()->SetWH(*w, *h);
	})));
}
