#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RenderLab.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
	}
	namespace Engine {
		class Scene;
	}
}

class RenderLab : public QMainWindow
{
	Q_OBJECT

public:
	RenderLab(QWidget *parent = Q_NULLPTR);

private slots:
	void on_btn_RenderStart_clicked();
	void on_btn_RenderStop_clicked();
	void on_btn_SaveRasterImg_clicked();
	void on_btn_SaveRayTracerImg_clicked();
	void UI_Op(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> op);

private:
	Ui::RenderLabClass ui;
	CppUtil::Basic::Ptr<CppUtil::Qt::PaintImgOpCreator::PaintImgOp> paintImgOp;
};
