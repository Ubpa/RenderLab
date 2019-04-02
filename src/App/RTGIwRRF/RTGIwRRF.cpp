#include "RTGIwRRF.h"

#include "RRF_Raster.h"

#include <UI/Hierarchy.h>
#include <UI/Attribute.h>
#include <UI/Setting.h>

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/Engine/Roamer.h>
#include <CppUtil/Engine/RTX_Renderer.h>
#include <CppUtil/Engine/PathTracer.h>
#include <CppUtil/Engine/Viewer.h>
#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/OptixAIDenoiser.h>

#include <CppUtil/Basic/CSV.h>
#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

#include <qdebug.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qevent.h>

#include <synchapi.h>

using namespace App;
using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;
using namespace Ui;

RTGIwRRF::~RTGIwRRF() {
	delete timer;
}

RTGIwRRF::RTGIwRRF(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags), timer(nullptr)
{
	ui.setupUi(this);

	Init();
}

void RTGIwRRF::UI_Op(Operation::Ptr op) {
	op->Run();
}

void RTGIwRRF::Init() {
	InitScene();
	InitRaster();
	InitTimer();
}

void RTGIwRRF::InitScene() {
	auto root = SObj::Load(ROOT_PATH+"data/SObjs/App/RTGIwRRF/CB_Glass.xml");
	scene = ToPtr(new Scene(root, "scene"));
}

void RTGIwRRF::InitRaster() {
	rrfRaster = ToPtr(new RRF_Raster(scene));
	roamer = ToPtr(new Roamer(ui.OGLW_Raster));
	roamer->SetLock(false);

	ui.OGLW_Raster->SetInitOp(ToPtr(new LambdaOp([=]() {
		roamer->Init();
		rrfRaster->Init();
	})));

	ui.OGLW_Raster->SetPaintOp(ToPtr(new LambdaOp([=]() {
		rrfRaster->Draw();
	})));
}

void RTGIwRRF::InitTimer() {
	delete timer;

	timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
	});

	const size_t fps = 30;
	timer->start(1000 / fps);
}
