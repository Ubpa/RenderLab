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

	namespace Qt {
		class OpThread;
	}
}

namespace App{
	class RRF_Raster;

	BETTER_ENUM(ENUM_ARG, int,
		notrootpath,
		sobj,
		maxdepth,
		samplenum,
		notdenoise,
		csv)

	BETTER_ENUM(ENUM_TYPE, int,
		FRAG_COLOR,
		POSITION,
		VIEW_DIR,
		NORMAL,
		MAT_COLOR,
		IOR_ROUGHNESS_ID)

	class RTGIwRRF : public QMainWindow
	{
		Q_OBJECT

	public:
		enum ENUM_TYPE {
			FRAG_COLOR,// direct illumination
			POSITION,
			VIEW_DIR,
			NORMAL,
			MAT_COLOR,
			IOR_ROUGHNESS_ID,
		};

	public:
		typedef std::map<ENUM_ARG, docopt::value> ArgMap;
		explicit RTGIwRRF(const ArgMap & argMap, QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

		virtual ~RTGIwRRF();

	public:
		CppUtil::Basic::Ptr<CppUtil::Engine::Scene> GetScene()const { return scene; }

		void UI_Op(CppUtil::Basic::Ptr<CppUtil::Basic::Operation> op);

	private:
		void Init();
		void InitTimer();
		void InitScene();
		void InitRaster();

		void SaveData();

	private:
		Ui::RTGIwRRFClass ui;
		CppUtil::Basic::Ptr<CppUtil::Qt::PaintImgOpCreator::PaintImgOp> paintImgOp;
		CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
		CppUtil::Basic::Ptr<App::RRF_Raster> rrfRaster;
		CppUtil::Basic::Ptr<CppUtil::Engine::Roamer> roamer;
		QTimer * timer;

		std::map<ENUM_TYPE, std::vector<float>> dataMap;

		const docopt::value & GetArg(ENUM_ARG arg) const;
		template<typename T>
		T GetArgAs(ENUM_ARG arg) const;
		const ArgMap argMap;
	};
}
