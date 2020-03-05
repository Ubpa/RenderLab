#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_RTGIwRRF.h"

#include <Qt/PaintImgOpCreator.h>

#include <Basic/Ptr.h>
#include <Basic/Timer.h>

#include <3rdParty/docopt/docopt.h>
#include <3rdParty/enum.h>

#include <map>

namespace Ubpa {
	namespace Basic {
		class Op;
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
		Ptr<Scene> GetScene()const { return scene; }

		void UI_Op(Ptr<Op> op);

	private:
		void Init();
		void InitTimer();
		void InitScene();
		void InitRaster();

	private:
		Ui::RTGIwRRFClass ui;
		Ptr<PaintImgOpCreator::PaintImgOp> paintImgOp;
		Ptr<Scene> scene;
		Ptr<App::RRF_Raster> rrfRaster;
		Ptr<Roamer> roamer;
		QTimer * timer;
		Timer timer4fps;
		int totalFrame;
		int fps;
	};
}
