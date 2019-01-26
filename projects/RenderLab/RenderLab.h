#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RenderLab.h"

class RenderLab : public QMainWindow
{
	Q_OBJECT

public:
	RenderLab(QWidget *parent = Q_NULLPTR);

private:
	Ui::RenderLabClass ui;
};
