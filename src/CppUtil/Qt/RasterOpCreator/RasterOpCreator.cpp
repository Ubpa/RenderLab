#include <CppUtil/Qt/RasterOpCreator.h>

#include "Defines.h"

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/OpenGL/VAO.h>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Texture.h>
#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/Camera.h>
#include <CppUtil/OpenGL/FBO.h>

#include <CppUtil/Basic/EventManager.h>
#include <CppUtil/Basic/OpNode.h>
#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/Cube.h>
#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/OpQueue.h>
#include <CppUtil/Basic/GStorage.h>

#include <ROOT_PATH.h>

#include <qevent.h>
#include <qcursor.h>
#include <QtWidgets/QApplication>
#include <qdebug.h>

using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace Define;
using namespace std;

const string rootPath = ROOT_PATH;

//---------------------------

RasterOpCreator::SceneOp::SceneOp(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), initOp(nullptr), paintOp(nullptr), resizeOp(nullptr), listenerInitOp(nullptr) { }

bool RasterOpCreator::SceneOp::SetOp() {
	if (pOGLW == nullptr)
		return false;

	pOGLW->SetInitOp(initOp);
	pOGLW->SetPaintOp(paintOp);
	pOGLW->SetResizeOp(resizeOp);
	if (listenerInitOp != nullptr)
		listenerInitOp->Run();
	return true;
}

Operation::Ptr RasterOpCreator::SceneOp::GetDefaultResizeOp() {
	auto defaultResizeOp = ToPtr(new LambdaOp([this](){
		auto pOGLW = this->GetOGLW();

		int w = pOGLW->w;
		int h = pOGLW->h;
		glViewport(0, 0, w, h);
	}));
	return defaultResizeOp;
}

CppUtil::Basic::Operation::Ptr RasterOpCreator::SceneOp::GetDefaultListenerInitOp() {
	auto listenerInitOp = ToPtr(new LambdaOp([this]() {
		// log mouse pos and track mouse
		auto MRB_PressOp = ToPtr(new LambdaOp([this]() {
			auto pOGWL = this->GetOGLW();

			pOGWL->setMouseTracking(true);

			pOGWL->Reg("lock", true);
			int x = pOGWL->x;
			int y = pOGWL->y;

			pOGWL->Reg("lockX", x);
			pOGWL->Reg("lockY", y);

			QApplication::setOverrideCursor(::Qt::BlankCursor);
		}));
		EventMngr::Reg(::Qt::RightButton, (void*)this->GetOGLW(), EventMngr::MOUSE_PRESS, MRB_PressOp);

		// lock mouse and rotate camera
		auto mouseMoveOp = ToPtr(new LambdaOp([this]() {
			auto pOGWL = this->GetOGLW();

			bool * lock;
			pOGWL->GetP("lock", lock);
			if (lock == nullptr || *lock == false)
				return;

			int x = pOGWL->x;
			int y = pOGWL->y;

			int lockX;
			int lockY;
			pOGWL->GetV("lockX", lockX);
			pOGWL->GetV("lockY", lockY);

			int xOffset = x - lockX;
			int yOffset = lockY - y;

			Camera * mainCamera;
			pOGLW->GetP("mainCamera", mainCamera);

			mainCamera->ProcessMouseMovement(xOffset, yOffset);

			QCursor::setPos(pOGWL->mapToGlobal(QPoint(lockX, lockY)));
		}));
		EventMngr::Reg(::Qt::NoButton, (void*)this->GetOGLW(), EventMngr::MOUSE_MOVE, mouseMoveOp);

		// release mouse cursor
		auto MRB_ReleaseOp = ToPtr(new LambdaOp([this]() {
			auto pOGWL = this->GetOGLW();

			pOGWL->setMouseTracking(false);

			pOGWL->Reg("lock", false);

			QApplication::restoreOverrideCursor();
		}));
		EventMngr::Reg(::Qt::RightButton, (void*)this->GetOGLW(), EventMngr::MOUSE_RELEASE, MRB_ReleaseOp);

		// wheel
		auto wheelOp = ToPtr(new LambdaOp([this]() {
			auto pOGWL = this->GetOGLW();

			int angle = pOGWL->angle;

			Camera * mainCamera;
			pOGLW->GetP("mainCamera", mainCamera);

			mainCamera->ProcessMouseScroll(angle*0.1f);
		}));
		EventMngr::Reg(::Qt::NoButton, (void*)this->GetOGLW(), EventMngr::MOUSE_WHEEL, wheelOp);

		// Move
		uint moveKey[] = { ::Qt::Key_W, ::Qt::Key_S, ::Qt::Key_A, ::Qt::Key_D, ::Qt::Key_Q, ::Qt::Key_E };
		for (uint i = 0; i < 6; i++) {
			auto op = ToPtr(new LambdaOp([this, moveKey, i]() {
				auto pOGLW = this->GetOGLW();
				Camera * mainCamera;
				pOGLW->GetP("mainCamera", mainCamera);
				mainCamera->ProcessKeyboard(Camera::ENUM_Movement(Camera::MOVE_FORWARD + i), 0.015f);
			}));

			EventMngr::Reg(moveKey[i], (void*)this->GetOGLW(), EventMngr::KB_PRESS, op);
		}
	}));

	return listenerInitOp;
}

//---------------------------

RasterOpCreator::RasterOpCreator(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW) { }

RasterOpCreator::SceneOp::Ptr RasterOpCreator::GenScenePaintOp(int n) {
	switch (n)
	{
	case 0: {
		return GenScenePaintOp_0();
		break;
	}
	case 1: {
		return GenScenePaintOp_1();
		break;
	}
	default:
		return nullptr;
		break;
	}
}

RasterOpCreator::SceneOp::Ptr RasterOpCreator::GenScenePaintOp_0() {
	// init
	SceneOp::Ptr sceneOp = ToPtr(new SceneOp(pOGLW));

	auto initOp = ToPtr(new LambdaOp([sceneOp]() {
		auto pOGLW = sceneOp->GetOGLW();

		Texture tex_wood(rootPath + texture_wood);
		pOGLW->Reg("tex_wood", tex_wood);
		Texture tex_earth(rootPath + texture_earth, true);
		pOGLW->Reg("tex_earth", tex_earth);

		Shader shader(rootPath + str_Basic_P2T2_vs, rootPath + str_Basic_T2_fs);
		shader.SetInt("texture0", 0);
		pOGLW->Reg("shader_screen", shader);

		VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });
		pOGLW->Reg("vao_screen", VAO_Screen);
	}));

	// raster paint op
	auto paintOp = ToPtr(new LambdaOp([sceneOp]() {
		auto pOGLW = sceneOp->GetOGLW();

		Texture * tex;
		Shader * shader;
		VAO * VAO_Screen;

		pOGLW->GetP("tex_wood", tex);
		pOGLW->GetP("shader_screen", shader);
		pOGLW->GetP("vao_screen", VAO_Screen);
		if (!tex || !shader || !VAO_Screen)
			return;

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		tex->Use(0);
		VAO_Screen->Draw(*shader);
	}));

	// resize op
	auto resizeOp = sceneOp->GetDefaultResizeOp();

	sceneOp->initOp = initOp;
	sceneOp->paintOp = paintOp;
	sceneOp->resizeOp = resizeOp;
	// register the sceneOp to let op work
	pOGLW->Reg("sceneOp_0", sceneOp);
	
	return sceneOp;
}

RasterOpCreator::SceneOp::Ptr RasterOpCreator::GenScenePaintOp_1() {
	// init
	SceneOp::Ptr sceneOp = ToPtr(new SceneOp(pOGLW));

	auto initOp = ToPtr(new LambdaOp([sceneOp]() {
		auto pOGLW = sceneOp->GetOGLW();

		Cube cube;

		//------------ 模型 . P3N3T2_Cube
		vector<VAO::VBO_DataPatch> P3N3T2_Cube_Vec_VBO_Data_Patch = {
			{cube.GetPosArr(), cube.GetPosArrSize(), 3},
			{cube.GetNormalArr(), cube.GetNormalArrSize(), 3},
			{cube.GetTexCoordsArr(), cube.GetTexCoordsArrSize(), 2},
		};
		VAO VAO_P3N3T2_Cube(P3N3T2_Cube_Vec_VBO_Data_Patch, cube.GetIndexArr(), cube.GetIndexArrSize());
		pOGLW->Reg("VAO_P3N3T2_Cube", VAO_P3N3T2_Cube);


		//------------ 模型 . P3T2_Cube
		vector<VAO::VBO_DataPatch> P3T2_Cube_Vec_VBO_Data_Patch = {
			{cube.GetPosArr(), cube.GetPosArrSize(), 3},
			{cube.GetTexCoordsArr(), cube.GetTexCoordsArrSize(), 2},
		};
		VAO VAO_P3T2_Cube(P3T2_Cube_Vec_VBO_Data_Patch, cube.GetIndexArr(), cube.GetIndexArrSize());
		pOGLW->Reg("VAO_P3T2_Cube", VAO_P3N3T2_Cube);
		

		//------------ 模型 . Screen
		VAO VAO_Screen(&(data_ScreenVertices[0]), sizeof(data_ScreenVertices), { 2,2 });
		pOGLW->Reg("VAO_Screen", VAO_Screen);


		//------------ BloomObj Shader
		string bloomObj_vs = rootPath + str_BloomObj_vs;
		string bloomObj_fs = rootPath + str_BloomObj_fs;
		Shader bloomObjShader(bloomObj_vs, bloomObj_fs);
		if (!bloomObjShader.IsValid()) {
			printf("ERROR: bloomObjShader load fail\n");
			return;
		}
		bloomObjShader.UniformBlockBind("Camera", 0);
		bloomObjShader.SetInt("diffuseTexture", 0);
		const uint lightNum = 4;
		for (uint i = 0; i < lightNum; i++) {
			bloomObjShader.SetVec3f("lights[" + to_string(i) + "].Position", data_LightPositions[i]);
			bloomObjShader.SetVec3f("lights[" + to_string(i) + "].Color", data_LightColors[i]);
		}
		pOGLW->Reg("bloomObjShader", bloomObjShader);

		//------------ BloomLight Shader
		string bloomLight_vs = rootPath + str_BloomLight_vs;
		string bloomLight_fs = rootPath + str_BloomLight_fs;
		Shader bloomLightShader(bloomLight_vs, bloomLight_fs);
		if (!bloomLightShader.IsValid()) {
			printf("ERROR: bloomLightShader load fail\n");
			return;
		}
		bloomLightShader.UniformBlockBind("Camera", 0);
		pOGLW->Reg("bloomLightShader", bloomLightShader);

		//------------ Blur Shader
		string blur_vs = rootPath + str_Blur_vs;
		string blur_fs = rootPath + str_Blur_fs;
		Shader blurShader(blur_vs, blur_fs);
		if (!blurShader.IsValid()) {
			printf("ERROR: blurShader load fail\n");
			return;
		}
		blurShader.SetInt("image", 0);
		pOGLW->Reg("blurShader", blurShader);

		//------------ Blend Shader
		string blend_vs = rootPath + str_Blend_vs;
		string blend_fs = rootPath + str_Blend_fs;
		Shader blendShader(blend_vs, blend_fs);
		if (!blendShader.IsValid()) {
			printf("ERROR: blendShader load fail\n");
			return;
		}
		blendShader.SetInt("scene", 0);
		blendShader.SetInt("bloomBlur", 1);
		float exposure = 1.0f;
		pOGLW->Reg("exposure", exposure);
		bool bloomEnable = true;
		pOGLW->Reg("bloomEnable", bloomEnable);
		pOGLW->Reg("blendShader", blendShader);

		//------------ 纹理
		const int textureNum = 1;
		Texture textures[textureNum];
		string imgPath[textureNum] = {
			rootPath + texture_box,
		};
		bool flip[textureNum] = { true };
		bool gammaCorrection[textureNum] = { true };
		for (uint i = 0; i < textureNum; i++) {
			if (!textures[i].Load(imgPath[i], flip[i], gammaCorrection[i])) {
				printf("ERROR: Load texture [%s] fail.\n", imgPath[i].c_str());
				return;
			}
		}
		pOGLW->Reg("textures[0]", textures[0]);

		//------------ camera
		Camera mainCamera;
		pOGLW->Reg("mainCamera", mainCamera);

		uint cameraUBO;
		glGenBuffers(1, &cameraUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
		glBufferData(GL_UNIFORM_BUFFER, 144, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		pOGLW->Reg("Camera", cameraUBO);

		//------------ HDR_Bloom 帧缓冲
		FBO FBO_HDR_Bloom(val_windowWidth, val_windowHeight, FBO::ENUM_TYPE_RGBF2_DEPTH);
		pOGLW->Reg("FBO_HDR_Bloom", FBO_HDR_Bloom);


		//------------ Blur 帧缓冲
		FBO blurFBOs[2] = {
			FBO(val_windowWidth, val_windowHeight, FBO::ENUM_TYPE_COLOR_FLOAT),
			FBO(val_windowWidth, val_windowHeight, FBO::ENUM_TYPE_COLOR_FLOAT)
		};
		pOGLW->Reg("blurFBOs[0]", blurFBOs[0]);
		pOGLW->Reg("blurFBOs[1]", blurFBOs[1]);
	}));

	// raster paint op
	auto paintOp = ToPtr(new LambdaOp([sceneOp]() {
		auto pOGLW = sceneOp->GetOGLW();

		// pointers
		VAO * VAO_P3N3T2_Cube;
		VAO * VAO_P3T2_Cube;
		VAO * VAO_Screen;
		Shader * bloomObjShader;
		Shader * bloomLightShader;
		Shader * blurShader;
		float * exposure;
		bool * bloomEnable;
		Shader * blendShader;
		Texture * textures[1];
		Camera * mainCamera;
		uint * cameraUBO;
		FBO * FBO_HDR_Bloom;
		FBO * blurFBOs[2];
		
		// val
		pOGLW->GetP("VAO_P3N3T2_Cube", VAO_P3N3T2_Cube);
		pOGLW->GetP("VAO_P3T2_Cube", VAO_P3T2_Cube);
		pOGLW->GetP("VAO_Screen", VAO_Screen);
		pOGLW->GetP("bloomObjShader", bloomObjShader);
		pOGLW->GetP("bloomLightShader", bloomLightShader);
		pOGLW->GetP("exposure", exposure);
		pOGLW->GetP("bloomEnable", bloomEnable);
		pOGLW->GetP("blurShader", blurShader);
		pOGLW->GetP("blendShader", blendShader);
		pOGLW->GetP("textures[0]", textures[0]);
		pOGLW->GetP("mainCamera", mainCamera);
		pOGLW->GetP("FBO_HDR_Bloom", FBO_HDR_Bloom);
		pOGLW->GetP("Camera", cameraUBO);
		pOGLW->GetP("blurFBOs[0]", blurFBOs[0]);
		pOGLW->GetP("blurFBOs[1]", blurFBOs[1]);

		// check
		if (!VAO_P3N3T2_Cube
			|| !VAO_P3T2_Cube
			|| !VAO_Screen
			|| !bloomObjShader
			|| !bloomLightShader
			|| !blurShader
			|| !exposure
			|| !bloomEnable
			|| !blendShader
			|| !textures[0]
			|| !mainCamera
			|| !cameraUBO
			|| !FBO_HDR_Bloom
			|| !blurFBOs[0]
			|| !blurFBOs[1]) {
			qDebug() << "get pointer fail";
			return;
		}

		//------------ 更新相机
		auto cameraUBO_Update = ToPtr(new LambdaOp([&]() {
			glBindBuffer(GL_UNIFORM_BUFFER, *cameraUBO);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(mainCamera->GetViewMatrix()));
			glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(mainCamera->GetProjectionMatrix()));
			glBufferSubData(GL_UNIFORM_BUFFER, 128, 12, glm::value_ptr(mainCamera->GetPos()));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}));

		auto updateOpQueue = new OpQueue;
		(*updateOpQueue) << cameraUBO_Update;


		//------------ 模型场景
		auto objOp = ToPtr(new LambdaOp([&]() {
			textures[0]->Use();
			bloomObjShader->SetVec3f("viewPos", mainCamera->GetPos());

			glm::mat4 model;
			// create one large cube that acts as the floor
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
			model = glm::scale(model, glm::vec3(25.0f, 1.0f, 25.0f));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);

			// then create multiple cubes as the scenery
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
			model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(2.0f));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
			model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(2.5f));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
			model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(2.0f));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
			bloomObjShader->SetMat4f("model", model);
			VAO_P3N3T2_Cube->Draw(*bloomObjShader);
		}));

		auto lightOp = ToPtr(new LambdaOp([&]() {
			for (uint i = 0; i < lightNum; i++) {
				glm::mat4 model(1.0f);
				model = glm::translate(model, data_LightPositions[i]);
				model = glm::scale(model, glm::vec3(0.5f));
				bloomLightShader->SetMat4f("model", model);
				bloomLightShader->SetVec3f("lightColor", data_LightColors[i]);
				VAO_P3T2_Cube->Draw(*bloomLightShader);
			}
		}));
		
		GLint viewport[4];
		auto bloomOp = new OpNode([&]() {
			glGetIntegerv(GL_VIEWPORT, viewport);
			FBO_HDR_Bloom->Use();
			glViewport(0, 0, val_windowWidth, val_windowHeight);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}, [&]() {
			FBO::UseDefault();
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		});
		(*bloomOp) << objOp << lightOp;

		auto blurOp = ToPtr(new LambdaOp([&]() {
			if (!*bloomEnable)
				return;

			glGetIntegerv(GL_VIEWPORT, viewport);
			glViewport(0, 0, val_windowWidth, val_windowHeight);

			bool horizontal = true;
			bool first_iteration = true;
			for (uint i = 0; i < 10; i++) {
				blurFBOs[horizontal]->Use();
				blurShader->SetBool("horizontal", horizontal);
				if (first_iteration)
					FBO_HDR_Bloom->GetColorTexture(1).Use();
				else
					blurFBOs[!horizontal]->GetColorTexture().Use();

				VAO_Screen->Draw(*blurShader);
				horizontal = !horizontal;
				first_iteration = false;
			}

			FBO::UseDefault();
			glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		}));

		auto blendOp = ToPtr(new LambdaOp([&]() {
			FBO_HDR_Bloom->GetColorTexture(0).Use(0);
			blurFBOs[0]->GetColorTexture().Use(1);
			blendShader->SetFloat("exposure", *exposure);
			blendShader->SetBool("bloomEnable", *bloomEnable);
			VAO_Screen->Draw(*blendShader);
		}));

		auto renderOp = new OpNode([]() {//init
			glEnable(GL_DEPTH_TEST);
			glClearColor(1.0f, 0.f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		});

		(*renderOp) << ToPtr(bloomOp) << blurOp << blendOp;

		//------------- 整合
		auto opQueue = new OpQueue;
		(*opQueue) << ToPtr(updateOpQueue) << ToPtr(renderOp);

		ToPtr(opQueue)->Run();
	}));

	// resize op
	auto resizeOp = ToPtr(new LambdaOp([sceneOp]() {
		auto pOGLW = sceneOp->GetOGLW();

		int w = pOGLW->w;
		int h = pOGLW->h;

		glViewport(0, 0, w, h);
		
		Camera * mainCamera;
		pOGLW->GetP("mainCamera", mainCamera);
		if (!mainCamera) {
			qDebug() << "get mainCamera fail\n";
			return;
		}
		mainCamera->SetRatioWH(w, h);
	}));

	// listenerInitOp

	auto listenerInitOp = sceneOp->GetDefaultListenerInitOp();

	sceneOp->initOp = initOp;
	sceneOp->paintOp = paintOp;
	sceneOp->resizeOp = resizeOp;
	sceneOp->listenerInitOp = listenerInitOp;
	// register the sceneOp to let op work
	pOGLW->Reg("sceneOp_1", sceneOp);

	return sceneOp;
}