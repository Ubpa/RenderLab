#include <CppUtil/Qt/PaintImgOpCreator.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

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

ptr<VAO> GenImgVAO(int imgW, int imgH, int winW, int winH);

PaintImgOpCreator::PaintImgOp::PaintImgOp(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), initOp(nullptr), paintOp(nullptr), resizeOp(nullptr), img(nullptr) { }

bool PaintImgOpCreator::PaintImgOp::SetOp(int w, int h) {
	if (pOGLW == nullptr)
		return false;

	img = ToPtr(new Image(w, h, 3));
	pOGLW->SetInitOp(initOp);
	pOGLW->SetPaintOp(paintOp);
	pOGLW->SetResizeOp(resizeOp);
	return true;
}

PaintImgOpCreator::PaintImgOpCreator(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW) { }

PaintImgOpCreator::PaintImgOp::Ptr PaintImgOpCreator::GenScenePaintOp() {
	// init
	PaintImgOp::Ptr paintImgOp = ToPtr(new PaintImgOp(pOGLW));

	auto initOp = ToPtr(new LambdaOp([paintImgOp]() {
		auto pOGLW = paintImgOp->GetOGLW();
		auto img = paintImgOp->GetImg();
		int imgW = img->GetWidth();
		int imgH = img->GetHeight();
		int winW = pOGLW->width();
		int winH = pOGLW->height();
		
		//------------ Texture
		Texture showImgTex(Texture::ENUM_TYPE_2D_DYNAMIC);
		pOGLW->Reg("showImgTex", showImgTex);

		//------------ VAO
		ptr<VAO> imgVAO = GenImgVAO(imgW, imgH, winW, winH);
		pOGLW->Reg("imgVAO", imgVAO);

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
		ptr<VAO> imgVAO;
		Shader * screenShader;

		pOGLW->GetP("showImgTex", showImgTex);
		pOGLW->GetV("imgVAO", imgVAO);
		pOGLW->GetP("screenShader", screenShader);

		if (!showImgTex || !imgVAO || !screenShader) {
			qDebug() << "ERROR: get pointer fail";
			return;
		}

		FBO::UseDefault();
		if (paintImgOp->GetImg() && paintImgOp->GetImg()->GetConstData()) {
			showImgTex->SetImg(*paintImgOp->GetImg());
			showImgTex->Use(0);
			imgVAO->Draw(*screenShader);
		}
	}));

	auto resizeOp = ToPtr(new LambdaOp([paintImgOp]() {
		auto pOGLW = paintImgOp->GetOGLW();
		auto img = paintImgOp->GetImg();
		int imgW = img->GetWidth();
		int imgH = img->GetHeight();

		int * w;
		int * h;
		pOGLW->GetP(RawAPI_OGLW::str_w, w);
		pOGLW->GetP(RawAPI_OGLW::str_h, h);
		if (!w || !h) {
			qDebug() << "get w or h fail\n";
			return;
		}
		glViewport(0, 0, *w, *h);

		ptr<VAO> imgVAO = GenImgVAO(imgW, imgH, *w, *h);
		pOGLW->Reg("imgVAO", imgVAO);
	}));

	paintImgOp->initOp = initOp;
	paintImgOp->paintOp = paintOp;
	paintImgOp->resizeOp = resizeOp;

	pOGLW->Reg("paintImgOp", paintImgOp);
	return paintImgOp;
}

ptr<VAO> GenImgVAO(int imgW, int imgH, int winW, int winH){
	float imgRatioWH = float(imgW) / float(imgH);
	float winRatioWH = float(winW) / float(winH);
	float minX, maxX, minY, maxY;
	if (winRatioWH > imgRatioWH) {
		float R = imgRatioWH / winRatioWH;
		minX = -R;
		maxX = R;
		minY = -1;
		maxY = 1;
	}
	else {
		float R = winRatioWH / imgRatioWH;
		minX = -1;
		maxX = 1;
		minY = -R;
		maxY = R;
	}

	float data[] = {
		minX, maxY,  0.0f, 1.0f,
		minX, minY,  0.0f, 0.0f,
		maxX, minY,  1.0f, 0.0f,

		minX, maxY,  0.0f, 1.0f,
		maxX, minY,  1.0f, 0.0f,
		maxX, maxY,  1.0f, 1.0f
	};

	ptr<VAO> imgVAO = ptr<VAO>(new VAO(&(data[0]), sizeof(data), { 2,2 }));
	return imgVAO;
}