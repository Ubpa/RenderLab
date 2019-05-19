#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/Basic/Sphere.h>
#include <CppUtil/Basic/Plane.h>
#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/EventManager.h>
#include <CppUtil/Basic/Op.h>

#include <qevent.h>
#include <qnamespace.h>

using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

const PtrC<TriMesh> RawAPI_OGLW::cube = TriMesh::GenCube();

RawAPI_OGLW::RawAPI_OGLW(QWidget* parent,Qt::WindowFlags f)
	: QOpenGLWidget(parent, f), resizeOp(nullptr), paintOp(nullptr), initOp(nullptr) {
	setFocusPolicy(Qt::ClickFocus);
}

RawAPI_OGLW::~RawAPI_OGLW() { }

void RawAPI_OGLW::SetInitOp(Ptr<Op> initOp) {
	this->initOp = initOp;
}

void RawAPI_OGLW::SetPaintOp(Ptr<Op> paintOp) {
	this->paintOp = paintOp;
}

void RawAPI_OGLW::SetResizeOp(Ptr<Op> resizeOp) {
	this->resizeOp = resizeOp;
}

void RawAPI_OGLW::initializeGL(){
	if (initOp != nullptr) {
		initOp->Run();
		initOp = nullptr;
	}

	InitShapeVAOs();
}

void RawAPI_OGLW::resizeGL(int w, int h) {
	if (resizeOp != nullptr) {
		this->w = w;
		this->h = h;
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
	EventMngr::GetInstance().Response(event->key(), (void*)this, EventMngr::KB_PRESS);
}

void RawAPI_OGLW::keyReleaseEvent(QKeyEvent *event) {
	EventMngr::GetInstance().Response(event->key(), (void*)this, EventMngr::KB_RELEASE);
}

void RawAPI_OGLW::mouseMoveEvent(QMouseEvent *event) {
	x = event->x();
	y = event->y();
	// event->button() is Qt::NoButton	0x00000000
	EventMngr::GetInstance().Response(event->button(), (void*)this, EventMngr::MOUSE_MOVE);
}

void RawAPI_OGLW::mousePressEvent(QMouseEvent *event) {
	x = event->x();
	y = event->y();
	EventMngr::GetInstance().Response(event->button(), (void*)this, EventMngr::MOUSE_PRESS);
}

void RawAPI_OGLW::mouseReleaseEvent(QMouseEvent *event) {
	x = event->x();
	y = event->y();
	EventMngr::GetInstance().Response(event->button(), (void*)this, EventMngr::MOUSE_RELEASE);
}

void RawAPI_OGLW::wheelEvent(QWheelEvent *event) {
	// 15 degree
	angle = event->angleDelta().y() / 8;
	EventMngr::GetInstance().Response(Qt::NoButton, (void*)this, EventMngr::MOUSE_WHEEL);
}

void RawAPI_OGLW::InitShapeVAOs() {
	Sphere sphere(50);
	vector<VAO::VBO_DataPatch> P3_Sphere_Vec_VBO_Data_Patch = {
		{sphere.GetPosArr(), sphere.GetPosArrSize(), 3},
		{sphere.GetNormalArr(), sphere.GetNormalArrSize(), 3},
		{sphere.GetTexCoordsArr(), sphere.GetTexCoordsArrSize(), 2},
		{sphere.GetTangentArr(), sphere.GetTangentArrSize(), 3},
	};
	VAO_P3N3T2T3_Sphere = VAO(P3_Sphere_Vec_VBO_Data_Patch, sphere.GetIndexArr(), sphere.GetIndexArrSize());

	auto plane = TriMesh::GenPlane();
	vector<VAO::VBO_DataPatch> P3_Plane_Vec_VBO_Data_Patch = {
		{plane->GetPositions().data()->Data(), static_cast<uint>(plane->GetPositions().size() * 3 * sizeof(float)), 3},
		{plane->GetNormals().data()->Data(), static_cast<uint>(plane->GetNormals().size() * 3 * sizeof(float)), 3},
		{plane->GetTexcoords().data()->Data(), static_cast<uint>(plane->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{plane->GetTangents().data()->Data(), static_cast<uint>(plane->GetTangents().size() * 3 * sizeof(float)), 3},
	};
	VAO_P3N3T2T3_Plane = VAO(P3_Plane_Vec_VBO_Data_Patch, plane->GetIndice().data(), static_cast<uint>(plane->GetIndice().size() * sizeof(uint)));
}

const VAO RawAPI_OGLW::GetVAO(ShapeType shapeType){
	switch (shapeType)
	{
	case ShapeType::Sphere:
		return VAO_P3N3T2T3_Sphere;
	case ShapeType::Plane:
		return VAO_P3N3T2T3_Plane;
	case ShapeType::Cube:
		return GetVAO(cube);
	default:
		printf("ERROR::MgrVAO::GetShapeVAO:\n"
			"\t""not support shape(%s)\n", shapeType._to_string());
		return VAO::inValid;
	}
}

const VAO RawAPI_OGLW::GetVAO(const PtrC<TriMesh> mesh) {
	auto target = mesh2VAO.find(mesh);
	if (target != mesh2VAO.end())
		return target->second;

	if (mesh == nullptr)
		return VAO::inValid;

	vector<VAO::VBO_DataPatch> P3_Mesh_Vec_VBO_Data_Patch = {
		{mesh->GetPositions().data()->Data(), static_cast<uint>(mesh->GetPositions().size() * 3 * sizeof(float)), 3},
		{mesh->GetNormals().data()->Data(), static_cast<uint>(mesh->GetNormals().size() * 3 * sizeof(float)), 3},
		{mesh->GetTexcoords().data()->Data(), static_cast<uint>(mesh->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{mesh->GetTangents().data()->Data(), static_cast<uint>(mesh->GetTangents().size() * 3 * sizeof(float)), 3},
	};

	VAO VAO_P3N3T2T3_Mesh(P3_Mesh_Vec_VBO_Data_Patch, mesh->GetIndice().data(), static_cast<uint>(mesh->GetIndice().size() * sizeof(uint)));
	mesh2VAO[mesh] = VAO_P3N3T2T3_Mesh;

	return VAO_P3N3T2T3_Mesh;
}

const Texture RawAPI_OGLW::GetTex(PtrC<Image> img) {
	auto target = img2tex.find(img);
	if (target != img2tex.end())
		return target->second;

	auto tex = Texture(img);
	img2tex[img] = tex;
	return tex;
}
