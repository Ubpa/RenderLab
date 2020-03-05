#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RenderLab.h"

#include <Qt/PaintImgOpCreator.h>
#include <Basic/Ptr.h>

namespace Ubpa {
	class Op;
	class Scene;
	class Viewer;
	class RTX_Renderer;
	class PathTracer;
}

using namespace Ubpa;

class RenderLab : public QMainWindow
{
	Q_OBJECT


public:
	RenderLab(QWidget *parent = Q_NULLPTR);

public:
	Ptr<Scene> GetScene()const { return scene; }

private slots:
	void on_btn_RenderStart_clicked();
	void on_btn_RenderStop_clicked();
	void on_btn_SaveRasterImg_clicked();
	void on_btn_SaveRayTracerImg_clicked();
	void on_btn_Denoise_clicked();

	void UI_Op(Ptr<Op> op);

private:
	void InitSetting();

private:
	Ui::RenderLabClass ui;
	Ptr<PaintImgOpCreator::PaintImgOp> paintImgOp;
	Ptr<Scene> scene;
	Ptr<Viewer> viewer;
	Ptr<RTX_Renderer> rtxRenderer;

private:
	// setting
	int maxDepth;
	int maxLoop;
};
