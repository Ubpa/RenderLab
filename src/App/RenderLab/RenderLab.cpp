#include "RenderLab.h"

#include "RasterSceneCreator.h"

#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/Basic/LambdaOp.h>

#include <ROOT_PATH.h>

#include <qdebug.h>
#include <qtimer.h>

using namespace CppUtil::Basic;
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

	RasterSceneCreator sc(ui.OGLW_Raster);
	auto rasterSceneOp = sc.GenScenePaintOp(1);

	// raytracer paint op
	auto paintOp1 = ToPtr(new LambdaOp([this]() {
		static int i = 0;
		i++;
		glClearColor(0.5f*(1+sinf(i*0.1f)), 0.5f*(1 + sinf(1+i*0.1f)), 0.5f*(1 + sinf(2+i*0.1f)), 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}));

	QTimer * timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});
	timer->start(33);

	rasterSceneOp->SetOp();
	ui.OGLW_RayTracer->SetPaintOp(paintOp1);
}
