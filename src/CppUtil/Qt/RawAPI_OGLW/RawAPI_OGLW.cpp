#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Basic/GStorage.h>

using namespace CppUtil::Qt;
using namespace CppUtil::Basic;

std::string RawAPI_OGLW::str_w = "resizeGL::width";
std::string RawAPI_OGLW::str_h = "resizeGL::height";

RawAPI_OGLW::RawAPI_OGLW(QWidget* parent, ::Qt::WindowFlags f)
	: QOpenGLWidget(parent, f), resizeOp(nullptr), paintOp(nullptr), initOp(nullptr) { }

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
}

void RawAPI_OGLW::paintGL() {
	if (paintOp != nullptr)
		paintOp->Run();
}
