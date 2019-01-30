#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Basic/GStorage.h>

#include <CppUtil/Basic/EventManager.h>

#include <qevent.h>
#include <qnamespace.h>

using namespace CppUtil::Qt;
using namespace CppUtil::Basic;

std::string RawAPI_OGLW::str_w = "resizeGL::width";
std::string RawAPI_OGLW::str_h = "resizeGL::height";
std::string RawAPI_OGLW::str_x = "resizeGL::x";
std::string RawAPI_OGLW::str_y = "resizeGL::y";
std::string RawAPI_OGLW::str_angle = "resizeGL::angle";

RawAPI_OGLW::RawAPI_OGLW(QWidget* parent, ::Qt::WindowFlags f)
	: QOpenGLWidget(parent, f), resizeOp(nullptr), paintOp(nullptr), initOp(nullptr) {
	setFocusPolicy(::Qt::ClickFocus);
}

RawAPI_OGLW::~RawAPI_OGLW() { }

void RawAPI_OGLW::SetInitOp(Operation::Ptr initOp) {
	this->initOp = initOp;
}

void RawAPI_OGLW::SetPaintOp(Operation::Ptr paintOp) {
	this->paintOp = paintOp;
}

void RawAPI_OGLW::SetResizeOp(Operation::Ptr resizeOp) {
	this->resizeOp = resizeOp;
}

void RawAPI_OGLW::initializeGL(){
	if (initOp != nullptr) {
		initOp->Run();
		initOp = nullptr;
	}
}

void RawAPI_OGLW::resizeGL(int w, int h) {
	if (resizeOp != nullptr) {
		Reg(str_w, w);
		Reg(str_h, h);
		resizeOp->Run();
	}
	else
		glViewport(0, 0, w, h);
}

void RawAPI_OGLW::paintGL() {
	if (initOp != nullptr) {
		initOp->Run();
		initOp = nullptr;
	}

	if (paintOp != nullptr)
		paintOp->Run();
}

void RawAPI_OGLW::keyPressEvent(QKeyEvent *event) {
	EventMngr::Response(event->key(), (void*)this, EventMngr::KB_PRESS);
}

void RawAPI_OGLW::keyReleaseEvent(QKeyEvent *event) {
	EventMngr::Response(event->key(), (void*)this, EventMngr::KB_RELEASE);
}

void RawAPI_OGLW::mouseMoveEvent(QMouseEvent *event) {
	Reg(str_x, event->x());
	Reg(str_y, event->y());
	// event->button() is Qt::NoButton	0x00000000
	EventMngr::Response(event->button(), (void*)this, EventMngr::MOUSE_MOVE);
}

void RawAPI_OGLW::mousePressEvent(QMouseEvent *event) {
	Reg(str_x, event->x());
	Reg(str_y, event->y());
	EventMngr::Response(event->button(), (void*)this, EventMngr::MOUSE_PRESS);
}

void RawAPI_OGLW::mouseReleaseEvent(QMouseEvent *event) {
	Reg(str_x, event->x());
	Reg(str_y, event->y());
	EventMngr::Response(event->button(), (void*)this, EventMngr::MOUSE_RELEASE);
}

void RawAPI_OGLW::wheelEvent(QWheelEvent *event) {
	// 15 degree
	Reg(str_angle, event->angleDelta().y()/8);
	EventMngr::Response(::Qt::NoButton, (void*)this, EventMngr::MOUSE_WHEEL);
}