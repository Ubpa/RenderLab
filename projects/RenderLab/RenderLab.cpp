#include "RenderLab.h"

RenderLab::RenderLab(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// hide titlebar of dock_Top
	QWidget* lTitleBar = ui.dock_Top->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	ui.dock_Top->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;
}
