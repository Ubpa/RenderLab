#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RTGIwRRF.h"

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Basic/Ptr.h>

#include <3rdParty/docopt/docopt.h>
#include <3rdParty/enum.h>

#include <map>

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

	namespace QT {
		class OpThread;
	}
}

namespace App{
	class RRF_Raster;

	class RTGIwRRF : public QMainWindow
	{
		Q_OBJECT

	public:
		explicit RTGIwRRF(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

		virtual ~RTGIwRRF();

	public:
		CppUtil::Basic::Ptr<CppUtil::Engine::Scene> GetScene()const { return scene; }

		void UI_Op(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> op);

	private:
		void Init();
		void InitTimer();
		void InitScene();
		void InitRaster();

	private:
		Ui::RTGIwRRFClass ui;
		CppUtil::Basic::Ptr<CppUtil::QT::PaintImgOpCreator::PaintImgOp> paintImgOp;
		CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
		CppUtil::Basic::Ptr<App::RRF_Raster> rrfRaster;
		CppUtil::Basic::Ptr<CppUtil::Engine::Roamer> roamer;
		QTimer * timer;
	};
}
