#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Basic/GStorage.h>

using namespace CppUtil::Qt;
using namespace CppUtil::Basic;

Operation::Ptr RawAPI_OGLW::initOp = nullptr;

RawAPI_OGLW::RawAPI_OGLW(QWidget* parent, ::Qt::WindowFlags f)
	: QOpenGLWidget(parent, f), paintOp(nullptr), init(false) { }

RawAPI_OGLW::~RawAPI_OGLW() { }

void RawAPI_OGLW::SetPaintOp(Operation::Ptr paintOp) {
	this->paintOp = paintOp;
}

void RawAPI_OGLW::SetInitOp(Operation::Ptr initOp) {
	RawAPI_OGLW::initOp = initOp;
}

void RawAPI_OGLW::initializeGL(){
	if (initOp != nullptr && !init) {
		GStorage<std::string>::GetInstance()->Register("objectname", objectName().toStdString());
		initOp->Run();
		init = true;
	}

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
