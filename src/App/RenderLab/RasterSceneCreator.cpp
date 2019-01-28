#include "RasterSceneCreator.h"

#include <ROOT_PATH.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/Shader.h>

#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/GStorage.h>

using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;

RasterSceneCreator::RasterSceneCreator(CppUtil::Qt::RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW) { }

RasterSceneCreator::SceneOp::SceneOp(CppUtil::Qt::RawAPI_OGLW * pOGLW, CppUtil::Basic::Operation::Ptr initOp, CppUtil::Basic::Operation::Ptr paintOp)
	: pOGLW(pOGLW), initOp(initOp), paintOp(paintOp) { }

bool RasterSceneCreator::SceneOp::SetOp() {
	if (pOGLW == nullptr)
		return false;

	pOGLW->SetPaintOp(paintOp);
	pOGLW->SetInitOp(initOp);
	return true;
}

RasterSceneCreator::SceneOp::Ptr RasterSceneCreator::GenScenePaintOp(int n) {
	switch (n)
	{
	case 0: {
		return GenScenePaintOp_0();
		break;
	}
	default:
		return nullptr;
		break;
	}
}

RasterSceneCreator::SceneOp::Ptr RasterSceneCreator::GenScenePaintOp_0() {
	// init
	SceneOp::Ptr sceneOp = ToPtr(new SceneOp(pOGLW));

	auto initOp = ToPtr(new LambdaOp([]() {
		std::string name;
		GS::GetV("objectname", name);

		Texture tex_wood(std::string(ROOT_PATH) + Define::texture_wood);
		GS::Reg(name + "tex_wood", tex_wood);
		Texture tex_earth(std::string(ROOT_PATH) + Define::texture_earth, true);
		GS::Reg(name + "tex_earth", tex_earth);

		Shader shader(std::string(ROOT_PATH) + Define::str_Basic_P2T2_vs, std::string(ROOT_PATH) + Define::str_Basic_T2_fs);
		shader.SetInt("texture0", 0);
		GS::Reg(name + "shader_screen", shader);

		VAO VAO_Screen(&(Define::data_ScreenVertices[0]), sizeof(Define::data_ScreenVertices), { 2,2 });
		GS::Reg(name + "vao_screen", VAO_Screen);
	}));

	// raster paint op
	auto paintOp = ToPtr(new LambdaOp([sceneOp]() {
		std::string name = sceneOp->pOGLW->objectName().toStdString();
		Texture * tex;
		Shader * shader;
		VAO * VAO_Screen;

		GS::GetP(name + "tex_wood", tex);
		GS::GetP(name + "shader_screen", shader);
		GS::GetP(name + "vao_screen", VAO_Screen);
		if (!tex || !shader || !VAO_Screen)
			return;

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		tex->Use(0);
		VAO_Screen->Draw(*shader);
	}));

	sceneOp->initOp = initOp;
	sceneOp->paintOp = paintOp;
	GS::Reg(pOGLW->objectName().toStdString()+"sceneOp_0", sceneOp);
	
	return sceneOp;
}