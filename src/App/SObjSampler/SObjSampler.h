#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SObjSampler.h"

#include <Qt/PaintImgOpCreator.h>
#include <Basic/Ptr.h>

#include <3rdParty/docopt/docopt.h>
#include <3rdParty/enum.h>

#include <map>

namespace CppUtil {
	namespace Basic {
		class Op;
	}

	namespace Engine {
		class Scene;
		class Roamer;
		class PathTracer;
	}

	namespace QT {
		class OpThread;
	}
}

namespace App{
	class SampleRaster;
	class RTX_Sampler;

	using namespace CppUtil;
	using namespace CppUtil::Basic;
	using namespace CppUtil::Engine;
	using namespace CppUtil::QT;

	BETTER_ENUM(ENUM_ARG, int,
		notrootpath,
		sobj,
		maxdepth,
		maxloop,
		samplenum,
		csv)

	BETTER_ENUM(ENUM_TYPE, int,
		FRAG_COLOR,
		POSITION,
		VIEW_DIR,
		NORMAL,
		MAT_COLOR,
		IOR_ROUGHNESS_ID)

	class SObjSampler : public QMainWindow
	{
		Q_OBJECT

	public:
		enum ENUM_TYPE {
			DirectIllum,
			POSITION,
			VIEW_DIR,
			NORMAL,
			MAT_COLOR,
			IOR_ROUGHNESS_ID,
		};

	public:
		typedef std::map<ENUM_ARG, docopt::value> ArgMap;
		explicit SObjSampler(const ArgMap & argMap, QWidget *parent = Q_NULLPTR,Qt::WindowFlags flags =Qt::WindowFlags());

		virtual ~SObjSampler();

	public:
		Ptr<Scene> GetScene()const { return scene; }

		void UI_Op(Ptr<Op> op);

	private:
		void Init();
		void InitTimer();
		void InitScene();
		void InitRaster();
		void InitRTX();

		void SaveData();

	private:
		Ui::SObjSamplerClass ui;
		Ptr<CppUtil::QT::PaintImgOpCreator::PaintImgOp> paintImgOp;
		Ptr<Scene> scene;
		Ptr<RTX_Sampler> rtxSampler;
		Ptr<CppUtil::QT::OpThread> drawImgThread;
		Ptr<SampleRaster> sampleRaster;
		Ptr<Roamer> roamer;
		QTimer * timer;

		volatile bool initDataMap;
		std::map<ENUM_TYPE, std::vector<float>> dataMap;

		const docopt::value & GetArg(ENUM_ARG arg) const;
		template<typename T>
		T GetArgAs(ENUM_ARG arg) const;
		const ArgMap argMap;
	};
}
