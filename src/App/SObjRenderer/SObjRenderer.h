#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SObjRenderer.h"

#include <Qt/PaintImgOpCreator.h>
#include <Basic/Ptr.h>

#include <3rdParty/enum.h>
#include <3rdParty/docopt/docopt.h>

namespace CppUtil {
	namespace Basic {
		class Op;
	}

	namespace Engine {
		class Scene;
		class Viewer;
		class RTX_Renderer;
		class PathTracer;
	}

	namespace QT {
		class OpThread;
	}
}

BETTER_ENUM(ENUM_ARG, int,
	notrootpath,
	sobj,
	maxdepth,
	samplenum,
	notdenoise,
	outpath)

BETTER_ENUM(ENUM_TYPE, int,
	FRAG_COLOR,
	POSITION,
	VIEW_DIR,
	NORMAL,
	MAT_COLOR,
	IOR_ROUGHNESS_ID)

class SObjRenderer : public QMainWindow
{
	Q_OBJECT

public:
	typedef std::map<ENUM_ARG, docopt::value> ArgMap;
	explicit SObjRenderer(const ArgMap & argMap, QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());

	virtual ~SObjRenderer();

public:
	CppUtil::Basic::Ptr<CppUtil::Engine::Scene> GetScene()const { return scene; }

	void UI_Op(CppUtil::Basic::Ptr<CppUtil::Basic::Op> op);

private:
	void Init();
	void InitTimer();
	void InitScene();
	void InitRaster();
	void InitRTX();

private:
	Ui::SObjRendererClass ui;
	CppUtil::Basic::Ptr<CppUtil::QT::PaintImgOpCreator::PaintImgOp> paintImgOp;
	CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene;
	CppUtil::Basic::Ptr<CppUtil::Engine::Viewer> viewer;
	CppUtil::Basic::Ptr<CppUtil::Engine::RTX_Renderer> rtxRenderer;
	CppUtil::Basic::Ptr<CppUtil::Engine::PathTracer> pathTracer;
	CppUtil::Basic::Ptr<CppUtil::QT::OpThread> drawImgThread;
	QTimer * timer;

private:
	const docopt::value & GetArg(ENUM_ARG arg) const;
	template<typename T>
	T GetArgAs(ENUM_ARG arg) const;
	const ArgMap argMap;
};
