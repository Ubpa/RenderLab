#include "RenderLab.h"

#include <CppUtil/Basic/LambdaOp.h>

using namespace CppUtil::Basic;

RenderLab::RenderLab(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// hide titlebar of dock_Top
	QWidget* lTitleBar = ui.dock_Top->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	ui.dock_Top->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;

	auto paintOp0 = ToPtr(new LambdaOp([]() {
		glClearColor(0, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}));

	auto paintOp1 = ToPtr(new LambdaOp([]() {
		glClearColor(1, 0, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}));

	ui.OGLW_Raster->SetPaintOp(paintOp0);
	ui.OGLW_RayTracer->SetPaintOp(paintOp1);
}
