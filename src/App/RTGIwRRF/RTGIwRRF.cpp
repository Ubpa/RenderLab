#include "RTGIwRRF.h"

#include "RRF_Raster.h"

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

template<>
bool RTGIwRRF::GetArgAs<bool>(ENUM_ARG arg) const {
	return GetArg(arg).asBool();
}

template<>
long RTGIwRRF::GetArgAs<long>(ENUM_ARG arg) const {
	return GetArg(arg).asLong();
}

template<>
int RTGIwRRF::GetArgAs<int>(ENUM_ARG arg) const {
	return static_cast<int>(GetArg(arg).asLong());
}

template<>
string RTGIwRRF::GetArgAs<string>(ENUM_ARG arg) const {
	return GetArg(arg).asString();
}

template<>
vector<string> RTGIwRRF::GetArgAs<vector<string>>(ENUM_ARG arg) const {
	return GetArg(arg).asStringList();
}

RTGIwRRF::~RTGIwRRF() {
	delete timer;
}

RTGIwRRF::RTGIwRRF(const ArgMap & argMap, QWidget *parent, Qt::WindowFlags flags)
	: argMap(argMap), QMainWindow(parent, flags), timer(nullptr)
{
	ui.setupUi(this);

	Init();
}

void RTGIwRRF::UI_Op(Operation::Ptr op) {
	op->Run();
}

void RTGIwRRF::Init() {
	InitScene();
	InitRaster();
	InitTimer();
}

void RTGIwRRF::InitScene() {
	bool isNotFromRootPath = GetArgAs<bool>(ENUM_ARG::notrootpath);
	string path = GetArgAs<string>(ENUM_ARG::sobj);
	string prefix = isNotFromRootPath ? "" : ROOT_PATH;

	auto root = SObj::Load(prefix + path);
	scene = ToPtr(new Scene(root, "scene"));
}

void RTGIwRRF::InitRaster() {
	rrfRaster = ToPtr(new RRF_Raster(scene));
	roamer = ToPtr(new Roamer(ui.OGLW_Raster));
	roamer->SetLock(true);

	ui.OGLW_Raster->SetInitOp(ToPtr(new LambdaOp([=]() {
		roamer->Init();
		rrfRaster->Init();
	})));

	auto paintOp = ToPtr(new OpQueue);

	paintOp->Push(ToPtr(new LambdaOp([=]() {
		rrfRaster->Draw();
	})));

	paintOp->Push(ToPtr(new LambdaOp([=]() {
		dataMap[ENUM_TYPE::FRAG_COLOR] = rrfRaster->GetData(RRF_Raster::ENUM_TYPE::FRAG_COLOR);
		dataMap[ENUM_TYPE::POSITION] = rrfRaster->GetData(RRF_Raster::ENUM_TYPE::POSITION);
		dataMap[ENUM_TYPE::VIEW_DIR] = rrfRaster->GetData(RRF_Raster::ENUM_TYPE::VIEW_DIR);
		dataMap[ENUM_TYPE::NORMAL] = rrfRaster->GetData(RRF_Raster::ENUM_TYPE::NORMAL);
		dataMap[ENUM_TYPE::MAT_COLOR] = rrfRaster->GetData(RRF_Raster::ENUM_TYPE::MAT_COLOR);
		dataMap[ENUM_TYPE::IOR_ROUGHNESS_ID] = rrfRaster->GetData(RRF_Raster::ENUM_TYPE::IOR_ROUGHNESS_ID);
	}, false)));

	ui.OGLW_Raster->SetPaintOp(paintOp);
}

void RTGIwRRF::InitTimer() {
	delete timer;

	timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
	});

	const size_t fps = 30;
	timer->start(1000 / fps);
}

void RTGIwRRF::SaveData() {
	static const vector<string> keys = {
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
		"ID",
		"IndirectIllum_R",
		"IndirectIllum_G",
		"IndirectIllum_B",
	};

	bool isNotFromRootPath = GetArgAs<bool>(ENUM_ARG::notrootpath);
	string path = GetArgAs<string>(ENUM_ARG::csv);
	string prefix = isNotFromRootPath ? "" : ROOT_PATH;

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

	csv.Save(prefix + path);
}

const docopt::value & RTGIwRRF::GetArg(ENUM_ARG arg) const {
	static const docopt::value invalid;

	auto target = argMap.find(arg);
	if (target == argMap.cend())
		return invalid;

	return target->second;
}
