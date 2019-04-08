#include "SObjRenderer.h"

#include <UI/Hierarchy.h>
#include <UI/Attribute.h>
#include <UI/Setting.h>

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/Engine/RTX_Renderer.h>
#include <CppUtil/Engine/PathTracer.h>
#include <CppUtil/Engine/Viewer.h>
#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/OptixAIDenoiser.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

#include <qdebug.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qevent.h>

#include <synchapi.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil::QT;
using namespace std;
using namespace Ui;

template<>
bool SObjRenderer::GetArgAs<bool>(ENUM_ARG arg) const {
	return GetArg(arg).asBool();
}

template<>
long SObjRenderer::GetArgAs<long>(ENUM_ARG arg) const {
	return GetArg(arg).asLong();
}

template<>
int SObjRenderer::GetArgAs<int>(ENUM_ARG arg) const {
	return static_cast<int>(GetArg(arg).asLong());
}

template<>
string SObjRenderer::GetArgAs<string>(ENUM_ARG arg) const {
	return GetArg(arg).asString();
}

template<>
vector<string> SObjRenderer::GetArgAs<vector<string>>(ENUM_ARG arg) const {
	return GetArg(arg).asStringList();
}


SObjRenderer::~SObjRenderer() {
	delete timer;
}

SObjRenderer::SObjRenderer(const ArgMap & argMap, QWidget *parent, Qt::WindowFlags flags)
	: argMap(argMap), QMainWindow(parent, flags), timer(nullptr)
{
	ui.setupUi(this);

	Init();
}

void SObjRenderer::UI_Op(Ptr<Op> op) {
	op->Run();
}

void SObjRenderer::Init() {
	InitScene();
	InitRaster();
	InitRTX();
	InitTimer();
}

void SObjRenderer::InitScene() {
	bool isNotFromRootPath = GetArgAs<bool>(ENUM_ARG::notrootpath);
	string path = GetArgAs<string>(ENUM_ARG::sobj);
	string prefix = isNotFromRootPath ? "" : ROOT_PATH;

	auto root = SObj::Load(prefix + path);
	scene = Scene::New(root, "scene");
}

void SObjRenderer::InitRaster() {
	viewer = Viewer::New(ui.OGLW_Raster, scene);
	viewer->SetLock(true);
}

void SObjRenderer::InitRTX() {
	int maxDepth = GetArgAs<int>(ENUM_ARG::maxdepth);
	auto generator = [=]()->Ptr<PathTracer>{
		auto pathTracer = PathTracer::New();
		pathTracer->maxDepth = maxDepth;

		return pathTracer;
	};

	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();
	paintImgOp->SetOp(512, 512);
	auto img = paintImgOp->GetImg();
	rtxRenderer = RTX_Renderer::New(generator);
	rtxRenderer->maxLoop = GetArgAs<int>(ENUM_ARG::samplenum);

	drawImgThread = OpThread::New(LambdaOp_New([=]() {
		rtxRenderer->Run(scene, img);
		if (!GetArgAs<bool>(ENUM_ARG::notdenoise))
			OptixAIDenoiser::GetInstance().Denoise(img);

		string path = GetArgAs<string>(ENUM_ARG::outpath);
		if (path.empty())
			return;

		bool isNotFromRootPath = GetArgAs<bool>(ENUM_ARG::notrootpath);
		string prefix = isNotFromRootPath ? "" : ROOT_PATH;
		img->SaveAsPNG(prefix + path);
	}));
	drawImgThread->start();
}

void SObjRenderer::InitTimer() {
	delete timer;

	timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});

	const size_t fps = 30;
	timer->start(1000 / fps);
}

const docopt::value & SObjRenderer::GetArg(ENUM_ARG arg) const {
	static const docopt::value invalid;

	auto target = argMap.find(arg);
	if (target == argMap.cend())
		return invalid;

	return target->second;
}
