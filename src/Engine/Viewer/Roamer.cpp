#include <Engine/Viewer/Roamer.h>

#include <Qt/RawAPI_OGLW.h>
#include <Qt/RawAPI_Define.h>

#include <OpenGL/Camera.h>

#include <Basic/Op/LambdaOp.h>
#include <Basic/EventManager.h>
#include <Basic/GStorage.h>

#include <QtWidgets/QApplication>

using namespace Ubpa;

using namespace std;

Roamer::Roamer(RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), camera(new Camera(pointf3(0,0.75,2.3))), lock(false) {
	GS::Reg("Roamer::camera", WPtr<Camera>(camera));
}

void Roamer::Init() {
	glGenBuffers(1, &cameraUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	glBufferData(GL_UNIFORM_BUFFER, 160, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, cameraUBO);
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
	auto MRB_PressOp = LambdaOp_New([this]() {
		if (this->lock)
			return;

		auto pOGWL = this->GetOGLW();

		pOGWL->setMouseTracking(true);

		pOGWL->Reg("lock", true);
		int x = pOGWL->x;
		int y = pOGWL->y;

		pOGWL->Reg("lockX", x);
		pOGWL->Reg("lockY", y);

		QApplication::setOverrideCursor(Qt::BlankCursor);
	}, true);
	EventMngr::GetInstance().Reg(Qt::RightButton, (void*)this->GetOGLW(), EventMngr::MOUSE_PRESS, MRB_PressOp);

	// lock mouse and rotate camera
	auto mouseMoveOp = LambdaOp_New([this]() {
		if (this->lock)
			return;

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
	}, true);
	EventMngr::GetInstance().Reg(Qt::NoButton, (void*)this->GetOGLW(), EventMngr::MOUSE_MOVE, mouseMoveOp);

	// release mouse cursor
	auto MRB_ReleaseOp = LambdaOp_New([this]() {
		if (this->lock)
			return;

		auto pOGWL = this->GetOGLW();

		pOGWL->setMouseTracking(false);

		pOGWL->Reg("lock", false);

		QApplication::restoreOverrideCursor();
	}, true);
	EventMngr::GetInstance().Reg(Qt::RightButton, (void*)this->GetOGLW(), EventMngr::MOUSE_RELEASE, MRB_ReleaseOp);

	// wheel
	auto wheelOp = LambdaOp_New([this]() {
		if (this->lock)
			return;

		auto pOGWL = this->GetOGLW();

		int angle = pOGWL->angle;

		this->GetCamera()->ProcessMouseScroll(angle*0.1f);
		UpdateCamera();
	}, true);
	EventMngr::GetInstance().Reg(Qt::NoButton, (void*)this->GetOGLW(), EventMngr::MOUSE_WHEEL, wheelOp);

	// Move
	size_t moveKey[] = {Qt::Key_W,Qt::Key_S,Qt::Key_A,Qt::Key_D,Qt::Key_Q,Qt::Key_E };
	for (size_t i = 0; i < 6; i++) {
		auto op = LambdaOp_New([this, moveKey, i]() {
			if (this->lock)
				return;

			auto pOGLW = this->GetOGLW();
			this->GetCamera()->ProcessKeyboard(Camera::ENUM_Movement(Camera::MOVE_FORWARD + i), 0.015f);
			UpdateCamera();
		}, true);

		EventMngr::GetInstance().Reg(moveKey[i], (void*)this->GetOGLW(), EventMngr::KB_PRESS, op);
	}

	// UpdateCamera
	auto updateCam = LambdaOp_New([this]() {
		UpdateCamera();
	}, true);
	EventMngr::GetInstance().Reg(0, camera, updateCam);
}

void Roamer::UpdateCamera() {
	glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
	auto viewMatrix = camera->GetViewMatrix();
	auto projectM = camera->GetProjectionMatrix();

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, viewMatrix.data());
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, projectM.data());
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 12, camera->GetPos().data());
	glBufferSubData(GL_UNIFORM_BUFFER, 140, 4, &camera->GetNearPlane());
	glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &camera->GetFarPlane());
	glBufferSubData(GL_UNIFORM_BUFFER, 148, 4, &camera->GetFOV());
	glBufferSubData(GL_UNIFORM_BUFFER, 152, 4, &camera->GetAspectRatio());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
