#include "SObjSampler.h"

#include "SampleRaster.h"

#include <UI/Hierarchy.h>
#include <UI/Attribute.h>
#include <UI/Setting.h>

#include <CppUtil/Qt/PaintImgOpCreator.h>
#include <CppUtil/Qt/OpThread.h>

#include <CppUtil/Engine/Roamer.h>
#include <CppUtil/Engine/RTX_Renderer.h>
#include <CppUtil/Engine/PathTracer.h>
#include <CppUtil/Engine/Viewer.h>
#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/OptixAIDenoiser.h>

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
using namespace CppUtil::Qt;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;
using namespace Ui;

SObjSampler::~SObjSampler() {
	delete timer;
}

SObjSampler::SObjSampler(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags), timer(nullptr)
{
	ui.setupUi(this);

	Init();
}

void SObjSampler::UI_Op(Operation::Ptr op) {
	op->Run();
}

void SObjSampler::Init() {
	InitScene();
	InitRaster();
	InitRTX();
	InitTimer();
}

void SObjSampler::InitScene() {
	auto root = SObj::Load(ROOT_PATH + "data/SObjs/CB_Glass.xml");
	scene = ToPtr(new Scene(root, "scene"));
}

void SObjSampler::InitRaster() {
	sampleRaster = ToPtr(new SampleRaster(scene));
	roamer = ToPtr(new Roamer(ui.OGLW_Raster));
	roamer->SetLock(true);

	ui.OGLW_Raster->SetInitOp(ToPtr(new LambdaOp([=]() {
		roamer->Init();
		sampleRaster->Init();
	})));

	auto paintOp = ToPtr(new OpQueue);

	paintOp->Push(ToPtr(new LambdaOp([=]() {
		sampleRaster->Draw();
	})));

	paintOp->Push(ToPtr(new LambdaOp([=]() {
		dataMap[ENUM_TYPE::FRAG_COLOR] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::FRAG_COLOR);
		dataMap[ENUM_TYPE::POSITION] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::POSITION);
		dataMap[ENUM_TYPE::VIEW_DIR] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::VIEW_DIR);
		dataMap[ENUM_TYPE::NORMAL] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::NORMAL);
		dataMap[ENUM_TYPE::MAT_COLOR] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::MAT_COLOR);
		dataMap[ENUM_TYPE::IOR_ROUGHNESS_ID] = sampleRaster->GetData(SampleRaster::ENUM_TYPE::IOR_ROUGHNESS_ID);

		/*
		auto imgFragColor = ToPtr(new Image(512, 512, 3));
		auto fragColors = dataMap[ENUM_TYPE::FRAG_COLOR];
		for (int row = 0; row < 512; row++) {
			for (int col = 0; col < 512; col++) {
				imgFragColor->SetPixel(row, col, glm::vec3(
					fragColors[(row * 512 + col) * 3 + 0],
					fragColors[(row * 512 + col) * 3 + 1],
					fragColors[(row * 512 + col) * 3 + 2]
				));
			}
		}
		imgFragColor->SaveAsPNG(ROOT_PATH + "data/out/data/fragColors.png");
		*/
	}, false)));

	ui.OGLW_Raster->SetPaintOp(paintOp);
}

void SObjSampler::InitRTX() {
	pathTracer = ToPtr(new PathTracer(scene));
	pathTracer->maxDepth = 5;

	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();
	paintImgOp->SetOp(512, 512);
	auto img = paintImgOp->GetImg();
	rtxRenderer = ToPtr(new RTX_Renderer(pathTracer));
	rtxRenderer->maxLoop = 1;

	drawImgThread = ToPtr(new OpThread([=]() {
		rtxRenderer->Run(img);
		OptixAIDenoiser::GetInstance().Denoise(img);

		SaveData();
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

	const size_t fps = 10;
	timer->start(1000 / fps);
}

void SObjSampler::SaveData() {
	vector<string> keys = {
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
		"Normal_x",
		"Normal_x",
		"MatColor_R",
		"MatColor_G",
		"MatColor_B",
		"IOR",
		"Roughness",
		"ID",
		"IndirectIllum_R",
		"IndirectIllum_G",
		"IndirectIllum_B",
	};
	
	CSV<float> csv(keys);
	vector<ENUM_TYPE> enumTypes = {
		//ENUM_TYPE::FRAG_COLOR,
		ENUM_TYPE::POSITION,
		ENUM_TYPE::VIEW_DIR,
		ENUM_TYPE::NORMAL,
		ENUM_TYPE::MAT_COLOR,
		ENUM_TYPE::IOR_ROUGHNESS_ID,
	};

	for (int row = 0; row < 512; row++) {
		for (int col = 0; col < 512; col++) {
			vector<float> lineVals;
			int idx = (row * 512 + col) * 3;

			vec3 localIllum(
				dataMap[ENUM_TYPE::FRAG_COLOR][idx + 0],
				dataMap[ENUM_TYPE::FRAG_COLOR][idx + 1],
				dataMap[ENUM_TYPE::FRAG_COLOR][idx + 2]
			);
			localIllum = sqrt(localIllum);

			lineVals.push_back(localIllum.r);
			lineVals.push_back(localIllum.g);
			lineVals.push_back(localIllum.b);

			for (auto enumType : enumTypes) {
				for (int channel = 0; channel < 3; channel++)
					lineVals.push_back(dataMap[enumType][idx + channel]);
			}

			vec3 globalIllum = paintImgOp->GetImg()->GetPixel_F(row, col);
			globalIllum = sqrt(globalIllum);

			vec3 indirectIllum = max(globalIllum - localIllum, 0.f);

			lineVals.push_back(indirectIllum.x);
			lineVals.push_back(indirectIllum.y);
			lineVals.push_back(indirectIllum.z);

			csv.AddLine(lineVals);
		}
	}

	csv.Save(ROOT_PATH + "data/out/data/train_data.csv");
}
