#include <CppUtil/Engine/Roamer.h>

#include <CppUtil/Qt/RawAPI_OGLW.h>
#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/OpenGL/Camera.h>

#include <CppUtil/Basic/LambdaOp.h>
#include <CppUtil/Basic/EventManager.h>

#include <glm/gtc/type_ptr.hpp>
#include <QtWidgets/QApplication>

using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;

Roamer::Roamer(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), camera(new Camera) { }

void Roamer::Init() {
	glGenBuffers(1, &cameraMatrixsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixsUBO);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraMatrixsUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	SetWH(pOGLW->width(), pOGLW->height());

	UpdateCamera();

	ListenerInit();
}

void Roamer::SetWH(int w, int h) {
	glViewport(0, 0, w, h);
	camera->SetRatioWH(w, h);
	UpdateCamera();
}

void Roamer::ListenerInit() {
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

		this->GetCamera()->ProcessMouseMovement(xOffset, yOffset);
		UpdateCamera();

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

		this->GetCamera()->ProcessMouseScroll(angle*0.1f);
		UpdateCamera();
	}));
	EventMngr::Reg(::Qt::NoButton, (void*)this->GetOGLW(), EventMngr::MOUSE_WHEEL, wheelOp);

	// Move
	size_t moveKey[] = { ::Qt::Key_W, ::Qt::Key_S, ::Qt::Key_A, ::Qt::Key_D, ::Qt::Key_Q, ::Qt::Key_E };
	for (size_t i = 0; i < 6; i++) {
		auto op = ToPtr(new LambdaOp([this, moveKey, i]() {
			auto pOGLW = this->GetOGLW();
			this->GetCamera()->ProcessKeyboard(OpenGL::Camera::ENUM_Movement(OpenGL::Camera::MOVE_FORWARD + i), 0.015f);
			UpdateCamera();
		}));

		EventMngr::Reg(moveKey[i], (void*)this->GetOGLW(), EventMngr::KB_PRESS, op);
	}
}

void Roamer::UpdateCamera() {
	glBindBuffer(GL_UNIFORM_BUFFER, cameraMatrixsUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(this->GetCamera()->GetProjectionMatrix()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
