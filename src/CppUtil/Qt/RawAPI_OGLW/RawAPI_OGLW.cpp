#include <CppUtil/Qt/RawAPI_OGLW.h>

using namespace CppUtil::Qt;
using namespace CppUtil::Basic;

RawAPI_OGLW::RawAPI_OGLW(QWidget* parent, ::Qt::WindowFlags f)
	: QOpenGLWidget(parent, f), paintOp(nullptr) {
}

RawAPI_OGLW::~RawAPI_OGLW(){

}

void RawAPI_OGLW::SetPaintOp(Operation::Ptr paintOp) {
	this->paintOp = paintOp;
}

void RawAPI_OGLW::initializeGL(){
	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void RawAPI_OGLW::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
}

void RawAPI_OGLW::paintGL() {
	if (paintOp != nullptr)
		paintOp->Run();
}

			