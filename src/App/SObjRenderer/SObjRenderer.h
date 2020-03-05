#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_SObjRenderer.h"

#include <Qt/PaintImgOpCreator.h>
#include <Basic/Ptr.h>

#include <3rdParty/enum.h>
#include <3rdParty/docopt/docopt.h>

namespace Ubpa {
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
	Ptr<Scene> GetScene()const { return scene; }

	void UI_Op(Ptr<Op> op);

private:
	void Init();
	void InitTimer();
	void InitScene();
	void InitRaster();
	void InitRTX();

private:
	Ui::SObjRendererClass ui;
	Ptr<PaintImgOpCreator::PaintImgOp> paintImgOp;
	Ptr<Scene> scene;
	Ptr<Viewer> viewer;
	Ptr<RTX_Renderer> rtxRenderer;
	Ptr<PathTracer> pathTracer;
	Ptr<OpThread> drawImgThread;
	QTimer * timer;

private:
	const docopt::value & GetArg(ENUM_ARG arg) const;
	template<typename T>
	T GetArgAs(ENUM_ARG arg) const;
	const ArgMap argMap;
};
