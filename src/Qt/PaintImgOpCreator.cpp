#include <Qt/PaintImgOpCreator.h>

#include <Qt/RawAPI_OGLW.h>
#include <Qt/RawAPI_Define.h>

#include <OpenGL/VAO.h>
#include <OpenGL/Texture.h>
#include <OpenGL/Shader.h>
#include <OpenGL/CommonDefine.h>
#include <OpenGL/FBO.h>

#include <Basic/Op/OpQueue.h>
#include <Basic/Op/LambdaOp.h>
#include <Basic/GStorage.h>
#include <Basic/Image.h>

#include <ROOT_PATH.h>

#include <qdebug.h>

using namespace Ubpa;

using namespace Define;
using namespace std;

const string rootPath = ROOT_PATH;

Ptr<VAO> GenImgVAO(int imgW, int imgH, int winW, int winH);

PaintImgOpCreator::PaintImgOp::PaintImgOp(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), initOp(nullptr), paintOp(nullptr), resizeOp(nullptr), img(nullptr) { }

bool PaintImgOpCreator::PaintImgOp::SetOp(int w, int h) {
	if (pOGLW == nullptr)
		return false;

	img = Image::New(w, h, 3);
	pOGLW->AddInitOp(initOp);
	pOGLW->AddPaintOp(paintOp);
	pOGLW->AddResizeOp(resizeOp);
	return true;
}

PaintImgOpCreator::PaintImgOpCreator(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW) { }

Ptr<PaintImgOpCreator::PaintImgOp> PaintImgOpCreator::GenScenePaintOp() {
	// init
	Ptr<PaintImgOp> paintImgOp = PaintImgOp::New(pOGLW);

	auto initOp = LambdaOp_New([paintImgOp]() {
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
		Ptr<VAO> imgVAO = GenImgVAO(imgW, imgH, winW, winH);
		pOGLW->Reg("imgVAO", imgVAO);

		//------------ Screen Shader
		string screen_vs = rootPath + str_Screen_vs;
		string screen_fs = rootPath + str_PostProcess_fs;
		Shader screenShader(screen_vs, screen_fs);
		if (!screenShader.IsValid()) {
			qDebug() << "ERROR: screenShader load fail";
			return;
		}
		screenShader.SetInt("texture0", 0);
		screenShader.SetBool("needGamma", true);
		screenShader.SetBool("needTonemap", true);
		pOGLW->Reg("screenShader", screenShader);
	}, false);

	auto paintOp = LambdaOp_New([paintImgOp]() {
		auto pOGLW = paintImgOp->GetOGLW();

		Texture * showImgTex;
		Ptr<VAO> imgVAO;
		Shader * screenShader;

		pOGLW->GetP("showImgTex", showImgTex);
		pOGLW->GetV("imgVAO", imgVAO);
		pOGLW->GetP("screenShader", screenShader);

		if (!showImgTex || !imgVAO || !screenShader) {
			qDebug() << "ERROR: get pointer fail";
			return;
		}

		FBO::UseDefault();
		if (paintImgOp->GetImg() && paintImgOp->GetImg()->GetData()) {
			showImgTex->SetImg(*paintImgOp->GetImg());
			showImgTex->Use(0);
			imgVAO->Draw(*screenShader);
		}
	}, true);

	auto resizeOp = LambdaOp_New([paintImgOp]() {
		auto pOGLW = paintImgOp->GetOGLW();
		auto img = paintImgOp->GetImg();
		int imgW = img->GetWidth();
		int imgH = img->GetHeight();

		int w = pOGLW->w;
		int h = pOGLW->h;

		glViewport(0, 0, w, h);

		Ptr<VAO> imgVAO = GenImgVAO(imgW, imgH, w, h);
		pOGLW->Reg("imgVAO", imgVAO);
	}, true);

	paintImgOp->initOp = initOp;
	paintImgOp->paintOp = paintOp;
	paintImgOp->resizeOp = resizeOp;

	pOGLW->Reg("paintImgOp", paintImgOp);
	return paintImgOp;
}

Ptr<VAO> GenImgVAO(int imgW, int imgH, int winW, int winH){
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

	Ptr<VAO> imgVAO = Ptr<VAO>(new VAO(&(data[0]), sizeof(data), { 2,2 }));
	return imgVAO;
}