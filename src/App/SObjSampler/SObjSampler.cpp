#include "SObjSampler.h"

#include "SampleRaster.h"
#include "RTX_Sampler.h"

#include <UI/Hierarchy.h>
#include <UI/Attribute.h>
#include <UI/Setting.h>

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/Engine/Roamer.h>
#include <CppUtil/Engine/PathTracer.h>
#include <CppUtil/Engine/Viewer.h>
#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/CmptCamera.h>
#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/OpenGL/Camera.h>

#include <CppUtil/Basic/CSV.h>
#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

#include <qdebug.h>
#include <qtimer.h>
#include <qfiledialog.h>
#include <qevent.h>

#include <synchapi.h>

using namespace App;
using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace std;
using namespace Ui;

template<>
bool SObjSampler::GetArgAs<bool>(ENUM_ARG arg) const {
	return GetArg(arg).asBool();
}

template<>
long SObjSampler::GetArgAs<long>(ENUM_ARG arg) const {
	return GetArg(arg).asLong();
}

template<>
int SObjSampler::GetArgAs<int>(ENUM_ARG arg) const {
	return static_cast<int>(GetArg(arg).asLong());
}

template<>
string SObjSampler::GetArgAs<string>(ENUM_ARG arg) const {
	return GetArg(arg).asString();
}

template<>
vector<string> SObjSampler::GetArgAs<vector<string>>(ENUM_ARG arg) const {
	return GetArg(arg).asStringList();
}

SObjSampler::~SObjSampler() {
	delete timer;
}

SObjSampler::SObjSampler(const ArgMap & argMap, QWidget *parent,Qt::WindowFlags flags)
	: argMap(argMap), QMainWindow(parent, flags), timer(nullptr)
{
	ui.setupUi(this);

	Init();
}

void SObjSampler::UI_Op(Ptr<Op> op) {
	op->Run();
}

void SObjSampler::Init() {
	InitScene();
	InitRaster();
	InitRTX();
	InitTimer();
}

void SObjSampler::InitScene() {
	bool isNotFromRootPath = GetArgAs<bool>(ENUM_ARG::notrootpath);
	string path = GetArgAs<string>(ENUM_ARG::sobj);
	string prefix = isNotFromRootPath ? "" : ROOT_PATH;

	auto root = SObj::Load(prefix + path);
	scene = Scene::New(root, "scene");
}

void SObjSampler::InitRaster() {
	initDataMap = false;

	sampleRaster = SampleRaster::New(scene);
	roamer = Roamer::New(ui.OGLW_Raster);
	roamer->SetLock(true);
	auto camera = scene->GetCmptCamera();
	auto transform = camera->GetSObj()->GetComponent<CmptTransform>();
	auto eulerAngle = transform->GetRotationEuler();
	roamer->GetCamera()->SetPose(transform->GetPosition(), - eulerAngle.y - 90, eulerAngle.x);

	ui.OGLW_Raster->SetInitOp(LambdaOp_New([=]() {
		roamer->OGL_Init();
		sampleRaster->OGL_Init();
	}));

	auto paintOp = OpQueue::New();

	paintOp->Push(LambdaOp_New([=]() {
		sampleRaster->Draw();
	}));

	paintOp->Push(LambdaOp_New([=]() {
		dataMap[ENUM_TYPE::DirectIllum] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::DirectIllum);
		dataMap[ENUM_TYPE::POSITION] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::POSITION);
		dataMap[ENUM_TYPE::VIEW_DIR] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::VIEW_DIR);
		dataMap[ENUM_TYPE::NORMAL] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::NORMAL);
		dataMap[ENUM_TYPE::MAT_COLOR] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::MAT_COLOR);
		dataMap[ENUM_TYPE::IOR_ROUGHNESS_ID] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::IOR_ROUGHNESS_ID);
		initDataMap = true;
	}, false));

	ui.OGLW_Raster->SetPaintOp(paintOp);
}

void SObjSampler::InitRTX() {
	int maxDepth = GetArgAs<int>(ENUM_ARG::maxdepth);
	auto generator = [=]()->Ptr<PathTracer> {
		auto pathTracer = PathTracer::New();
		pathTracer->maxDepth = maxDepth;

		return pathTracer;
	};

	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();
	paintImgOp->SetOp(512, 512);
	int maxLoop = GetArgAs<int>(ENUM_ARG::maxloop);
	int sampleNum = GetArgAs<int>(ENUM_ARG::samplenum);
	rtxSampler = RTX_Sampler::New(generator, maxLoop, sampleNum);

	drawImgThread = OpThread::New(LambdaOp_New([&]() {
		rtxSampler->Run(scene, paintImgOp->GetImg());
		Sleep(100);

		while (!initDataMap)
			;
		
		/*
		auto img = ToPtr(new Image(512, 512, 3));
		auto directIllum = dataMap[ENUM_TYPE::DirectIllum];
		for (int row = 0; row < 512; row++) {
			for (int col = 0; col < 512; col++) {
				for (int c = 0; c < 3; c++) {
					img->At(col, row, c) = clamp<uByte>(255 * directIllum[(row * 512 + col) * 3 + c], 0, 255);
				}
			}
		}
		img->SaveAsPNG(ROOT_PATH + "data/out/directIllum.png");
		paintImgOp->GetImg()->SaveAsPNG(ROOT_PATH + "data/out/globalIllum.png");
		*/

		SaveData();

		QApplication::quit();
	}));
	drawImgThread->start();
}

void SObjSampler::InitTimer() {
	delete timer;

	timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});

	const size_t fps = 30;
	timer->start(1000 / fps);
}

void SObjSampler::SaveData() {
	static const vector<string> keys = {
		"ID",
		"DirectIllum_R",
		"DirectIllum_G",
		"DirectIllum_B",
		"Position_x",
		"Position_y",
		"Position_z",
		"ViewDir_x",
		"ViewDir_y",
		"ViewDir_z",
		"Normal_x",
		"Normal_y",
		"Normal_z",
		"MatColor_R",
		"MatColor_G",
		"MatColor_B",
		"IOR",
		"Roughness",
		"IndirectIllum_R",
		"IndirectIllum_G",
		"IndirectIllum_B",
	};

	bool isNotFromRootPath = GetArgAs<bool>(ENUM_ARG::notrootpath);
	string path = GetArgAs<string>(ENUM_ARG::csv);
	string prefix = isNotFromRootPath ? "" : ROOT_PATH;

	CSV<float> csv(keys);

	//auto indirectImg = ToPtr(new Image(512, 512, 3));
	//auto directImg = ToPtr(new Image(512, 512, 3));
	//auto globalImg = ToPtr(new Image(512, 512, 3));
	map<int, string> ID2name;
	for (auto & job : rtxSampler->GetJobs()) {
		for (auto & pixel : job) {
			int col = pixel.x;
			int row = pixel.y;

			vector<float> lineVals;
			int idx = (row * 512 + col) * 3;

			float ID = dataMap[ENUM_TYPE::IOR_ROUGHNESS_ID][idx + 2];
			const string name = scene->GetName(ID);
			if (name == "")
				continue;

			
			ID2name[ID] = name;

			float ior = dataMap[ENUM_TYPE::IOR_ROUGHNESS_ID][idx + 0];
			float roughness = dataMap[ENUM_TYPE::IOR_ROUGHNESS_ID][idx + 1];
			lineVals.push_back(ID);

			RGBf directIllum(
				dataMap[ENUM_TYPE::DirectIllum][idx + 0],
				dataMap[ENUM_TYPE::DirectIllum][idx + 1],
				dataMap[ENUM_TYPE::DirectIllum][idx + 2]
			);

			lineVals.push_back(directIllum.r);
			lineVals.push_back(directIllum.g);
			lineVals.push_back(directIllum.b);

			for (int channel = 0; channel < 3; channel++)
				lineVals.push_back(dataMap[ENUM_TYPE::POSITION][idx + channel]);

			for (int channel = 0; channel < 3; channel++)
				lineVals.push_back(dataMap[ENUM_TYPE::VIEW_DIR][idx + channel]);

			for (int channel = 0; channel < 3; channel++) {
				float val = dataMap[ENUM_TYPE::NORMAL][idx + channel];
				
				// Ïû³ý¹Ø¼üÎó²î
				if (abs(val) > 0.999f)
					val = Math::sgn(val);
				else if (abs(val) < 0.001f)
					val = 0;

				lineVals.push_back(val);
			}

			for (int channel = 0; channel < 3; channel++)
				lineVals.push_back(dataMap[ENUM_TYPE::MAT_COLOR][idx + channel]);

			RGBf globalIllum = paintImgOp->GetImg()->GetPixel(col, row);

			RGBf indirectIllum = globalIllum - directIllum;
			indirectIllum.r = max(indirectIllum.r, 0.f);
			indirectIllum.g = max(indirectIllum.g, 0.f);
			indirectIllum.b = max(indirectIllum.b, 0.f);

			lineVals.push_back(ior);
			lineVals.push_back(roughness);

			lineVals.push_back(indirectIllum.x);
			lineVals.push_back(indirectIllum.y);
			lineVals.push_back(indirectIllum.z);

			csv.AddLine(lineVals);
		}
	}
	//indirectImg->SaveAsPNG(ROOT_PATH + "data/out/indirectIllum.png");
	//directImg->SaveAsPNG(ROOT_PATH + "data/out/directIllum.png");
	//globalImg->SaveAsPNG(ROOT_PATH + "data/out/global.png");

	csv.Save(prefix + path);
	File idMapFile(prefix + path + "_ID_name.txt", File::WRITE);
	for (auto & pair : ID2name)
		idMapFile.Printf("%d : %s\n", pair.first, pair.second.c_str());
	idMapFile.Close();

	printf("Save data complete\n");
}

const docopt::value & SObjSampler::GetArg(ENUM_ARG arg) const {
	static const docopt::value invalid;

	auto target = argMap.find(arg);
	if (target == argMap.cend())
		return invalid;

	return target->second;
}
