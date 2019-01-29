#include "RenderLab.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/RasterOpCreator.h>
#include <CppUtil/Qt/RawAPI_Define.h>
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
	auto paintImgOp = pioc.GenScenePaintOp(1024, 768);
	paintImgOp->SetOp();

	
	auto drawImgFunc = [paintImgOp]() {
		auto img = paintImgOp->GetImg();
		auto scene = SceneCreator::Gen(RTX::SceneCreator::LOTS_OF_BALLS);
		RTX_Renderer rtxRenderer(scene, img);
		rtxRenderer.Run();
	};

	std::thread drawImg(drawImgFunc);
	drawImg.detach();
}
