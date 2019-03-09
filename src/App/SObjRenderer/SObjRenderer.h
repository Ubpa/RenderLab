#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SObjRenderer.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
	}
	namespace Engine {
		class Scene;
		class Viewer;
		class RTX_Renderer;
		class PathTracer;
	}
}

class SObjRenderer : public QMainWindow
{
	Q_OBJECT

public:
	SObjRenderer(QWidget *parent = Q_NULLPTR);

public:
	CppUtil::Basic::Ptr<CppUtil::Engine::Scene> GetScene()const { return scene; }

	void UI_Op(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> op);

private:
	Ui::SObjRendererClass ui;
	CppUtil::Basic::Ptr<CppUtil::Qt::PaintImgOpCreator::PaintImgOp> paintImgOp;
	CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
	CppUtil::Basic::Ptr<CppUtil::Engine::Viewer> viewer;
	CppUtil::Basic::Ptr<CppUtil::Engine::RTX_Renderer> rtxRenderer;
	CppUtil::Basic::Ptr<CppUtil::Engine::PathTracer> pathTracer;

};
