#include "SObjSampler.h"

#include "SampleRaster.h"

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

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

#include <qdebug.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qevent.h>

#include <synchapi.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Qt;
using namespace std;
using namespace Ui;

SObjSampler::~SObjSampler() {
	delete timer;
}

SObjSampler::SObjSampler(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags), timer(nullptr)
{
	ui.setupUi(this);

	Init();
}

void SObjSampler::UI_Op(Operation::Ptr op) {
	op->Run();
}

void SObjSampler::Init() {
	InitScene();
	InitRaster();
	InitRTX();
	InitTimer();
}

void SObjSampler::InitScene() {
	auto root = SObj::Load(ROOT_PATH + "data/SObjs/CB_Glass.xml");
	scene = ToPtr(new Scene(root, "scene"));
}

void SObjSampler::InitRaster() {
	sampleRaster = ToPtr(new App::SampleRaster(scene));
	roamer = ToPtr(new Roamer(ui.OGLW_Raster));
	roamer->SetLock(true);

	ui.OGLW_Raster->SetInitOp(ToPtr(new LambdaOp([=]() {
		roamer->Init();
		sampleRaster->Init();
	})));

	ui.OGLW_Raster->SetPaintOp(ToPtr(new LambdaOp([=]() {
		sampleRaster->Draw();
	})));
}

void SObjSampler::InitRTX() {
	pathTracer = ToPtr(new PathTracer(scene));
	pathTracer->maxDepth = 5;

	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();
	paintImgOp->SetOp(512, 512);
	auto img = paintImgOp->GetImg();
	rtxRenderer = ToPtr(new RTX_Renderer(pathTracer));
	rtxRenderer->maxLoop = 1;

	drawImgThread = ToPtr(new OpThread([=]() {
		rtxRenderer->Run(img);
		OptixAIDenoiser::GetInstance().Denoise(img);
	}));
	drawImgThread->start();
}

void SObjSampler::InitTimer() {
	delete timer;

	timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});

	const size_t fps = 10;
	timer->start(1000 / fps);
}
