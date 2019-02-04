#include "RenderLab.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/RasterOpCreator.h>
#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/Engine/RTX_Renderer.h>
#include <CppUtil/Engine/RayTracer.h>
#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Camera.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Math.h>

#include <qdebug.h>
#include <qtimer.h>
#include <qfiledialog.h>

#include <thread>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::Qt;

class TestRayTracer : public RayTracer {
	HEAP_OBJ_SETUP(TestRayTracer)

public:
	using RayTracer::RayTracer;

	virtual glm::vec3 Trace(CppUtil::Basic::Ptr<Ray> ray) {
		return glm::vec3(Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F(), Math::Rand_F()*Math::Rand_F());
	}
};

RenderLab::RenderLab(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// hide titlebar of dock_Top
	QWidget* lTitleBar = ui.dock_Top->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	ui.dock_Top->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;

	// update per frame
	QTimer * timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});

	const size_t fps = 60;
	timer->start(1000 / fps);

	// raster
	RasterOpCreator roc(ui.OGLW_Raster);
	auto rasterSceneOp = roc.GenScenePaintOp(1);
	rasterSceneOp->SetOp();

	// raytracer
	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();
}

void RenderLab::on_btn_RenderStart_clicked(){
	ui.btn_RenderStart->setEnabled(false);
	ui.btn_RenderStop->setEnabled(true);
	ui.btn_SaveRayTracerImg->setEnabled(true);

	OpThread::Ptr drawImgThread = ToPtr(new OpThread);
	drawImgThread->UIConnect(this, &RenderLab::UI_Op);

	auto drawImgOp = ToPtr(new LambdaOp([this, drawImgThread]() {
		paintImgOp->SetOp(1024, 768);
		auto img = paintImgOp->GetImg();
		auto sobj = ToPtr(new SObj(nullptr, "test_sobj"));
		auto camera = ToPtr(new Camera(sobj));
		auto scene = ToPtr(new Scene(sobj, "test_scene"));
		auto testRayTracer = ToPtr(new TestRayTracer(scene));
		auto rtxRenderer = ToPtr(new RTX_Renderer(testRayTracer));

		OpThread::Ptr controller = ToPtr(new OpThread);
		controller->UIConnect(this, &RenderLab::UI_Op);
		auto controllOp = ToPtr(new LambdaOp([this, drawImgThread, controller, rtxRenderer]() {
			while (!drawImgThread->IsStop()) {
				controller->UI_Op_Run([&, this]() {
					ui.rtxProgress->setValue(rtxRenderer->ProgressRate() * ui.rtxProgress->maximum());
				});
				Sleep(100);
			}
			rtxRenderer->Stop();
		}));
		controller->SetOp(controllOp);
		controller->start();

		rtxRenderer->Run(img);
		controller->terminate();
		drawImgThread->UI_Op_Run([this, rtxRenderer]() {
			ui.btn_RenderStart->setEnabled(true);
			ui.btn_RenderStop->setEnabled(false);
			ui.rtxProgress->setValue(rtxRenderer->ProgressRate() * ui.rtxProgress->maximum());
		});
	}));
	drawImgThread->SetOp(drawImgOp);
	GS::Reg("drawImgThread", drawImgThread);

	drawImgThread->start();
}

void RenderLab::on_btn_RenderStop_clicked() {
	ui.btn_RenderStop->setEnabled(false);
	OpThread::Ptr drawImgThread;
	GS::GetV("drawImgThread", drawImgThread);
	if (!drawImgThread)
		return;
	drawImgThread->Stop();
}

void RenderLab::on_btn_SaveRayTracerImg_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("±£´æÍ¼Æ¬"),
		"./",
		tr("Image Files (*.png)"));
	if (fileName.isEmpty())
		return;
	
	// should not use grabFramebuffer to get the image because of the border
	Image::Ptr img = ToPtr(new Image(*paintImgOp->GetImg()));
	img->SaveAsPNG(fileName.toStdString(), true);
}

void RenderLab::on_btn_SaveRasterImg_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("±£´æÍ¼Æ¬"),
		"./",
		tr("Image Files (*.png)"));
	if (fileName.isEmpty())
		return;

	ui.OGLW_Raster->grabFramebuffer().save(fileName);
}

void RenderLab::UI_Op(Operation::Ptr op) {
	op->Run();
}