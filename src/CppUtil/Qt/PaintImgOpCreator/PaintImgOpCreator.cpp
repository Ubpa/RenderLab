#include <CppUtil/Qt/PaintImgOpCreator.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/FBO.h>

#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Image.h>

#include <ROOT_PATH.h>

#include <qdebug.h>

using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace Define;
using namespace std;

const string rootPath = ROOT_PATH;

PaintImgOpCreator::PaintImgOp::PaintImgOp(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), initOp(nullptr), paintOp(nullptr), resizeOp(nullptr), img(nullptr) { }

bool PaintImgOpCreator::PaintImgOp::SetOp() {
	if (pOGLW == nullptr)
		return false;

	pOGLW->SetInitOp(initOp);
	pOGLW->SetPaintOp(paintOp);
	pOGLW->SetResizeOp(resizeOp);
	return true;
}

PaintImgOpCreator::PaintImgOpCreator(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW) { }

PaintImgOpCreator::PaintImgOp::Ptr PaintImgOpCreator::GenScenePaintOp(int w, int h) {
	// init
	PaintImgOp::Ptr paintImgOp = ToPtr(new PaintImgOp(pOGLW));

	auto initOp = ToPtr(new LambdaOp([paintImgOp, w, h]() {
		auto pOGLW = paintImgOp->GetOGLW();

		//------------ Image
		// paintImgOp->img = ToPtr(new Image(w, h, 3));
		paintImgOp->img = ToPtr(new Image((rootPath + texture_wood).c_str()));
		
		//------------ Texture
		Texture showImgTex(Texture::ENUM_TYPE_2D_DYNAMIC);
		pOGLW->Reg("showImgTex", showImgTex);

		//------------ VAO
		VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });
		pOGLW->Reg("VAO_Screen", VAO_Screen);

		//------------ Screen Shader
		string screen_vs = rootPath + str_Screen_vs;
		string screen_fs = rootPath + str_Screen_fs;
		Shader screenShader(screen_vs, screen_fs);
		if (!screenShader.IsValid()) {
			qDebug() << "ERROR: screenShader load fail";
			return;
		}
		screenShader.SetInt("texture0", 0);
		pOGLW->Reg("screenShader", screenShader);
	}));

	auto paintOp = ToPtr(new LambdaOp([paintImgOp]() {
		auto pOGLW = paintImgOp->GetOGLW();

		Texture * showImgTex;
		VAO * VAO_Screen;
		Shader * screenShader;

		pOGLW->GetP("showImgTex", showImgTex);
		pOGLW->GetP("VAO_Screen", VAO_Screen);
		pOGLW->GetP("screenShader", screenShader);

		if (!showImgTex || !VAO_Screen || !screenShader) {
			qDebug() << "ERROR: get pointer fail";
			return;
		}

		FBO::UseDefault();
		showImgTex->SetImg(*paintImgOp->GetImg());
		showImgTex->Use(0);
		VAO_Screen->Draw(*screenShader);
	}));

	paintImgOp->initOp = initOp;
	paintImgOp->paintOp = paintOp;
	pOGLW->Reg("paintImgOp", paintImgOp);
	return paintImgOp;
}