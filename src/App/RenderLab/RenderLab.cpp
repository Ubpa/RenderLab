#include "RenderLab.h"

#include "GenScene.h"

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

RenderLab::RenderLab(QWidget *parent)
	: QMainWindow(parent), maxDepth(5), maxLoop(20)
{
	ui.setupUi(this);

	// hide titlebar of dock_Top
	QWidget* lTitleBar = ui.dock_Top->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	ui.dock_Top->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;

	// update per frame
	QTimer * timer = new QTimer;
	timer->callOnTimeout([this]() {
		ui.OGLW_Raster->update();
		ui.OGLW_RayTracer->update();
	});

	const size_t fps = 60;
	timer->start(1000 / fps);

	auto root = SObj::Load(ROOT_PATH + "data/SObjs/CB_Glass.xml");
	scene = Scene::New(root);

	// viewer
	viewer = Viewer::New(ui.OGLW_Raster, scene, RasterType::DirectIllum);

	// raytracer
	PaintImgOpCreator pioc(ui.OGLW_RayTracer);
	paintImgOp = pioc.GenScenePaintOp();

	auto generator = [&]()->Ptr<PathTracer>{
		auto pathTracer = PathTracer::New();
		pathTracer->maxDepth = maxDepth;

		return pathTracer;
	};

	rtxRenderer = RTX_Renderer::New(generator);
	rtxRenderer->maxLoop = maxLoop;
	
	// init ui

	Hierarchy::GetInstance()->Init(scene, ui.tree_Hierarchy);

	Attribute::GetInstance()->Init(ui.tbox_Attribute);

	InitSetting();
}

void RenderLab::on_btn_RenderStart_clicked(){
	ui.btn_RenderStart->setEnabled(false);
	ui.btn_RenderStop->setEnabled(true);
	ui.btn_SaveRayTracerImg->setEnabled(true);
	ui.btn_Denoise->setEnabled(false);

	ui.tree_Hierarchy->setEnabled(false);
	ui.frame_Setting->setEnabled(false);
	ui.tbox_Attribute->setEnabled(false);

	auto drawImgThread = OpThread::New();
	drawImgThread->UIConnect(this, &RenderLab::UI_Op);

	auto drawImgOp = LambdaOp_New([=]() {
		paintImgOp->SetOp(1024, 768);
		auto img = paintImgOp->GetImg();

		auto controller = OpThread::New();
		controller->UIConnect(this, &RenderLab::UI_Op);
		auto controllOp = LambdaOp_New([=]() {
			while (!drawImgThread->IsStop()) {
				controller->UI_Op_Run(LambdaOp_New([&, this]() {
					ui.rtxProgress->setValue(rtxRenderer->ProgressRate() * ui.rtxProgress->maximum());
				}));
				Sleep(100);
			}
			rtxRenderer->Stop();
		});
		controller->SetOp(controllOp);
		controller->start();

		rtxRenderer->Run(scene, img);

		controller->terminate();
		drawImgThread->UI_Op_Run(LambdaOp_New([=]() {
			ui.btn_RenderStart->setEnabled(true);
			ui.btn_RenderStop->setEnabled(false);

			ui.tree_Hierarchy->setEnabled(true);
			ui.frame_Setting->setEnabled(true);
			ui.tbox_Attribute->setEnabled(true);
			ui.btn_Denoise->setEnabled(true);
			//ui.rtxProgress->setValue(rtxRenderer->ProgressRate() * ui.rtxProgress->maximum());
		}));
	});
	drawImgThread->SetOp(drawImgOp);
	GS::Reg("drawImgThread", drawImgThread);

	drawImgThread->start();
}

void RenderLab::on_btn_RenderStop_clicked() {
	ui.btn_RenderStop->setEnabled(false);
	Ptr<OpThread> drawImgThread;
	GS::GetV("drawImgThread", drawImgThread);
	if (!drawImgThread)
		return;
	drawImgThread->Stop();
}

void RenderLab::on_btn_SaveRayTracerImg_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("±£´æÍ¼Æ¬"),
		"./",
		tr("Image Files (*.png)"));
	if (fileName.isEmpty())
		return;
	
	// should not use grabFramebuffer to get the image because of the border
	auto img = Image::New(*paintImgOp->GetImg());
	img->SaveAsPNG(fileName.toStdString(), true);
}

void RenderLab::on_btn_SaveRasterImg_clicked() {
	QString fileName = QFileDialog::getSaveFileName(this,
		tr("±£´æÍ¼Æ¬"),
		"./",
		tr("Image Files (*.png)"));
	if (fileName.isEmpty())
		return;

	ui.OGLW_Raster->grabFramebuffer().save(fileName);
}

void RenderLab::on_btn_Denoise_clicked() {
	ui.btn_RenderStart->setEnabled(false);
	ui.btn_SaveRayTracerImg->setEnabled(false);
	ui.btn_Denoise->setEnabled(false);

	auto denoiseThread = OpThread::New();
	denoiseThread->UIConnect(this, &RenderLab::UI_Op);
	auto denoiseOp = LambdaOp_New([=]() {
		OptixAIDenoiser::GetInstance().Denoise(paintImgOp->GetImg());
		denoiseThread->UI_Op(LambdaOp_New([&]() {
			ui.btn_RenderStart->setEnabled(true);
			ui.btn_SaveRayTracerImg->setEnabled(true);
			ui.btn_Denoise->setEnabled(true);
		}));
	});
	denoiseThread->SetOp(denoiseOp);
	denoiseThread->start();
}

void RenderLab::UI_Op(Ptr<Op> op) {
	op->Run();
}

void RenderLab::InitSetting() {
	auto setting = Setting::GetInstance();
	setting->Init(ui.frame_Setting);

	setting->AddTitle("[ RTX_Renderer ]");
	setting->AddEditVal("- Sample Num", maxLoop, 1, 1024, [&](int val) {
		rtxRenderer->maxLoop = val;
	});
	
	setting->AddTitle("[ PathTracer ]");
	setting->AddEditVal("- Max Depth", maxDepth, 1, 100, [&](int val) {
		maxDepth = val;
	});
}
