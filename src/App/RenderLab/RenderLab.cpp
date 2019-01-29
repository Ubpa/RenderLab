#include "RenderLab.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/RasterOpCreator.h>
#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/RTX/SceneCreator.h>
#include <CppUtil/RTX/RTX_Renderer.h>

#include <CppUtil/Basic/Image.h>

#include <CppUtil/Basic/LambdaOp.h>

#include <qdebug.h>
#include <qtimer.h>

#include <thread>

using namespace CppUtil::Basic;
using namespace RTX;
using namespace CppUtil::Qt;

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
	ui.OGLW_Raster->setFocusPolicy(Qt::ClickFocus);
	RasterOpCreator roc(ui.OGLW_Raster);
	auto rasterSceneOp = roc.GenScenePaintOp(1);
	rasterSceneOp->SetOp();

	// raytracer
	ui.OGLW_RayTracer->setFocusPolicy(Qt::ClickFocus);
	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();
}

void RenderLab::on_btn_RenderStart_clicked(){
	ui.btn_RenderStart->setEnabled(false);
	ui.btn_RenderStop->setEnabled(true);

	OpThread::Ptr drawImgThread = ToPtr(new OpThread());

	auto drawImgOp = ToPtr(new LambdaOp([this, drawImgThread]() {
		paintImgOp->SetOp(1024, 768);
		auto scene = SceneCreator::Gen(RTX::SceneCreator::LOTS_OF_BALLS, 1024, 768);
		auto img = paintImgOp->GetImg();
		RTX_Renderer rtxRenderer(scene, img);

		OpThread::Ptr controller = ToPtr(new OpThread(ToPtr(new LambdaOp([this, drawImgThread, &rtxRenderer]() {
			while (!drawImgThread->IsStop())
				Sleep(100);
			rtxRenderer.Stop();
		}))));
		controller->start();

		rtxRenderer.Run();
		qDebug() << "rtx is finished or stop\n";
	}));
	drawImgThread->SetOp(drawImgOp);

	GS::Reg("drawImgThread", drawImgThread);
	drawImgThread->start();
}

void RenderLab::on_btn_RenderStop_clicked() {
	ui.btn_RenderStart->setEnabled(true);
	ui.btn_RenderStop->setEnabled(false);
	OpThread::Ptr drawImgThread;
	GS::GetV("drawImgThread", drawImgThread);
	if (!drawImgThread)
		return;
	drawImgThread->Stop();
}
