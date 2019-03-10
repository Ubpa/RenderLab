#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SObjSampler.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
	}

	namespace Engine {
		class Scene;
		class Roamer;
		class RTX_Renderer;
		class PathTracer;
	}

	namespace Qt {
		class OpThread;
	}
}

namespace App{
	class SampleRaster;
}

class SObjSampler : public QMainWindow
{
	Q_OBJECT

public:
	explicit SObjSampler(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

	virtual ~SObjSampler();

public:
	CppUtil::Basic::Ptr<CppUtil::Engine::Scene> GetScene()const { return scene; }

	void UI_Op(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> op);

private:
	void Init();
	void InitTimer();
	void InitScene();
	void InitRaster();
	void InitRTX();

private:
	Ui::SObjSamplerClass ui;
	CppUtil::Basic::Ptr<CppUtil::Qt::PaintImgOpCreator::PaintImgOp> paintImgOp;
	CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
	CppUtil::Basic::Ptr<CppUtil::Engine::RTX_Renderer> rtxRenderer;
	CppUtil::Basic::Ptr<CppUtil::Engine::PathTracer> pathTracer;
	CppUtil::Basic::Ptr<CppUtil::Qt::OpThread> drawImgThread;
	CppUtil::Basic::Ptr<App::SampleRaster> sampleRaster;
	CppUtil::Basic::Ptr<CppUtil::Engine::Roamer> roamer;
	QTimer * timer;
};
