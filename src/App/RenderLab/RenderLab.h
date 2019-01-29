#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RenderLab.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>

class RenderLab : public QMainWindow
{
	Q_OBJECT

public:
	RenderLab(QWidget *parent = Q_NULLPTR);

private slots:
	void on_btn_RenderStart_clicked();
	void on_btn_RenderStop_clicked();

private:
	Ui::RenderLabClass ui;
	CppUtil::Basic::Ptr<CppUtil::Qt::PaintImgOpCreator::PaintImgOp> paintImgOp;
};
