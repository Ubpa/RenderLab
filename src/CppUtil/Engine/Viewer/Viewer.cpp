#include <CppUtil/Engine/Viewer.h>

#include <CppUtil/Engine/Raster.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/OpenGL/Shader.h>
#include <CppUtil/OpenGL/CommonDefine.h>
#include <CppUtil/OpenGL/Camera.h>

#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/EventManager.h>

#include <qevent.h>
#include <qcursor.h>
#include <QtWidgets/QApplication>
#include <qdebug.h>

#include <ROOT_PATH.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace glm;
using namespace std;

const string rootPath = ROOT_PATH;

Viewer::Viewer(RawAPI_OGLW * pOGLW, Scene::Ptr scene)
	: pOGLW(pOGLW), scene(scene), raster(ToPtr(new Raster(pOGLW, scene))), camera(new OpenGL::Camera) {
	pOGLW->SetInitOp(ToPtr(new LambdaOp([this]() {
		glGenBuffers(1, &cameraMatrixsUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixsUBO);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraMatrixsUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		this->ListenerInit();
		this->raster->Init();
	})));

	pOGLW->SetPaintOp(ToPtr(new LambdaOp([this]() {
		//------------ 更新相机
		glBindBuffer(GL_UNIFORM_BUFFER, this->GetCamUBO());
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(this->GetCamera()->GetViewMatrix()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(this->GetCamera()->GetProjectionMatrix()));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		this->GetRaster()->Draw();
	})));

	pOGLW->SetResizeOp(ToPtr(new LambdaOp([this]() {
		auto pOGLW = this->GetOGLW();

		int * w;
		int * h;
		pOGLW->GetP(RawAPI_OGLW::str_w, w);
		pOGLW->GetP(RawAPI_OGLW::str_h, h);
		if (!w || !h) {
			qDebug() << "get w or h fail\n";
			return;
		}
		glViewport(0, 0, *w, *h);
		this->GetCamera()->SetRatioWH(*w, *h);
	})));
}

void Viewer::ListenerInit() {
	// log mouse pos and track mouse
	auto MRB_PressOp = ToPtr(new LambdaOp([this]() {
		auto pOGWL = this->GetOGLW();

		pOGWL->setMouseTracking(true);

		pOGWL->Reg("lock", true);
		int x;
		int y;
		pOGWL->GetV(RawAPI_OGLW::str_x, x);
		pOGWL->GetV(RawAPI_OGLW::str_y, y);

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

		int x;
		int y;
		pOGWL->GetV(RawAPI_OGLW::str_x, x);
		pOGWL->GetV(RawAPI_OGLW::str_y, y);

		int lockX;
		int lockY;
		pOGWL->GetV("lockX", lockX);
		pOGWL->GetV("lockY", lockY);

		int xOffset = x - lockX;
		int yOffset = lockY - y;

		this->GetCamera()->ProcessMouseMovement(xOffset, yOffset);

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

		int angle;
		pOGWL->GetV(RawAPI_OGLW::str_angle, angle);

		this->GetCamera()->ProcessMouseScroll(angle*0.1f);
	}));
	EventMngr::Reg(::Qt::NoButton, (void*)this->GetOGLW(), EventMngr::MOUSE_WHEEL, wheelOp);

	// Move
	size_t moveKey[] = { ::Qt::Key_W, ::Qt::Key_S, ::Qt::Key_A, ::Qt::Key_D, ::Qt::Key_Q, ::Qt::Key_E };
	for (size_t i = 0; i < 6; i++) {
		auto op = ToPtr(new LambdaOp([this, moveKey, i]() {
			auto pOGLW = this->GetOGLW();
			this->GetCamera()->ProcessKeyboard(OpenGL::Camera::ENUM_Movement(OpenGL::Camera::MOVE_FORWARD + i), 0.015f);
		}));

		EventMngr::Reg(moveKey[i], (void*)this->GetOGLW(), EventMngr::KB_PRESS, op);
	}
}
