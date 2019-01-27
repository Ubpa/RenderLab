#include <ROOT_PATH.h>
#include "RenderLab.h"

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/Basic/GStorage.h>

#include <qdebug.h>

using namespace CppUtil::Basic;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Qt;

RenderLab::RenderLab(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// hide titlebar of dock_Top
	QWidget* lTitleBar = ui.dock_Top->titleBarWidget();
	QWidget* lEmptyWidget = new QWidget();
	ui.dock_Top->setTitleBarWidget(lEmptyWidget);
	delete lTitleBar;
	
	// init
	auto initOp = ToPtr(new LambdaOp([]() {
		std::string name = *GStorage<std::string>::GetInstance()->GetPtr("objectname");

		Texture tex_wood(std::string(ROOT_PATH) + Define::texture_wood);
		GStorage<Texture>::GetInstance()->Register(name + "tex_wood", tex_wood);
		Texture tex_earth(std::string(ROOT_PATH) + Define::texture_earth, true);
		GStorage<Texture>::GetInstance()->Register(name + "tex_earth", tex_earth);

		Shader shader(std::string(ROOT_PATH) + Define::str_Basic_P2T2_vs, std::string(ROOT_PATH) + Define::str_Basic_T2_fs);
		shader.SetInt("texture0", 0);
		GStorage<Shader>::GetInstance()->Register(name + "shader_screen", shader);

		VAO VAO_Screen(&(Define::data_ScreenVertices[0]), sizeof(Define::data_ScreenVertices), { 2,2 });
		GStorage<VAO>::GetInstance()->Register(name + "vao_screen", VAO_Screen);
	}));

	RawAPI_OGLW::SetInitOp(initOp);

	// raster paint op
	auto paintOp0 = ToPtr(new LambdaOp([this]() {
		std::string name = ui.OGLW_Raster->objectName().toStdString();
		Texture * tex = GStorage<Texture>::GetInstance()->GetPtr(name + "tex_wood");
		Shader * shader = GStorage<Shader>::GetInstance()->GetPtr(name + "shader_screen");
		VAO * VAO_Screen = GStorage<VAO>::GetInstance()->GetPtr(name + "vao_screen");
		if (!tex || !shader || !VAO_Screen)
			return;

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		tex->Use(0);
		VAO_Screen->Draw(*shader);
	}));

	// raytracer paint op
	auto paintOp1 = ToPtr(new LambdaOp([this]() {
		std::string name = ui.OGLW_RayTracer->objectName().toStdString();
		Texture * tex = GStorage<Texture>::GetInstance()->GetPtr(name + "tex_earth");
		Shader * shader = GStorage<Shader>::GetInstance()->GetPtr(name + "shader_screen");
		VAO * VAO_Screen = GStorage<VAO>::GetInstance()->GetPtr(name + "vao_screen");
		if (!tex || !shader || !VAO_Screen)
			return;

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		tex->Use(0);
		VAO_Screen->Draw(*shader);
	}));
	
	ui.OGLW_Raster->SetPaintOp(paintOp0);
	ui.OGLW_RayTracer->SetPaintOp(paintOp1);
}
