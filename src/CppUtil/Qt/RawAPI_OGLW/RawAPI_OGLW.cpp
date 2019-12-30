#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Engine/TriMesh.h>

#include <CppUtil/Qt/RawAPI_Define.h>

#include <CppUtil/OpenGL/CommonDefine.h>

#include <CppUtil/Basic/Sphere.h>
#include <CppUtil/Basic/Plane.h>
#include <CppUtil/Basic/Capsule.h>

#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/EventManager.h>
#include <CppUtil/Basic/OpQueue.h>

#include <qevent.h>
#include <qnamespace.h>

using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

const PtrC<TriMesh> RawAPI_OGLW::cube = TriMesh::GenCube();

RawAPI_OGLW::RawAPI_OGLW(QWidget* parent,Qt::WindowFlags f)
	: QOpenGLWidget(parent, f), resizeOpQueue(OpQueue::New()), paintOpQueue(OpQueue::New()), initOpQueue(OpQueue::New()) {
	setFocusPolicy(Qt::ClickFocus);
}

RawAPI_OGLW::~RawAPI_OGLW() { }

void RawAPI_OGLW::AddInitOp(Ptr<Op> initOp) {
	initOpQueue->Push(initOp);
}

void RawAPI_OGLW::AddPaintOp(Ptr<Op> paintOp) {
	paintOpQueue->Push(paintOp);
}

void RawAPI_OGLW::AddResizeOp(Ptr<Op> resizeOp) {
	resizeOpQueue->Push(resizeOp);
}

void RawAPI_OGLW::initializeGL(){
	initOpQueue->Run();

	InitShapeVAOs();
}

void RawAPI_OGLW::resizeGL(int w, int h) {
	if (!resizeOpQueue->IsEmpty()) {
		this->w = w;
		this->h = h;
		resizeOpQueue->Run();
	}
	else
		glViewport(0, 0, w, h);
}

void RawAPI_OGLW::paintGL() {
	initOpQueue->Run();

	paintOpQueue->Run();
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
	// sphere
	Sphere sphere(50);
	vector<VAO::VBO_DataPatch> P3_Sphere_Vec_VBO_Data_Patch = {
		{sphere.GetPosArr(), sphere.GetPosArrSize(), 3},
		{sphere.GetNormalArr(), sphere.GetNormalArrSize(), 3},
		{sphere.GetTexCoordsArr(), sphere.GetTexCoordsArrSize(), 2},
		{sphere.GetTangentArr(), sphere.GetTangentArrSize(), 3},
	};
	VAO_P3N3T2T3_Sphere = VAO(P3_Sphere_Vec_VBO_Data_Patch, sphere.GetIndexArr(), sphere.GetIndexArrSize());

	// plane
	auto plane = TriMesh::GenPlane();
	vector<VAO::VBO_DataPatch> P3_Plane_Vec_VBO_Data_Patch = {
		{plane->GetPositions().data()->Data(), static_cast<uint>(plane->GetPositions().size() * 3 * sizeof(float)), 3},
		{plane->GetNormals().data()->Data(), static_cast<uint>(plane->GetNormals().size() * 3 * sizeof(float)), 3},
		{plane->GetTexcoords().data()->Data(), static_cast<uint>(plane->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{plane->GetTangents().data()->Data(), static_cast<uint>(plane->GetTangents().size() * 3 * sizeof(float)), 3},
	};
	VAO_P3N3T2T3_Plane = VAO(P3_Plane_Vec_VBO_Data_Patch, plane->GetIndice().data(), static_cast<uint>(plane->GetIndice().size() * sizeof(uint)));

	// screen
	VAO_P3T2_Screen = VAO(Define::data_ScreenVertices, sizeof(Define::data_ScreenVertices), { 2,2 });

	// disk
	auto disk = TriMesh::GenDisk();
	vector<VAO::VBO_DataPatch> P3_Disk_Vec_VBO_Data_Patch = {
		{disk->GetPositions().data()->Data(), static_cast<uint>(disk->GetPositions().size() * 3 * sizeof(float)), 3},
		{disk->GetNormals().data()->Data(), static_cast<uint>(disk->GetNormals().size() * 3 * sizeof(float)), 3},
		{disk->GetTexcoords().data()->Data(), static_cast<uint>(disk->GetTexcoords().size() * 2 * sizeof(float)), 2},
		{disk->GetTangents().data()->Data(), static_cast<uint>(disk->GetTangents().size() * 3 * sizeof(float)), 3},
	};
	VAO_P3N3T2T3_Disk = VAO(P3_Disk_Vec_VBO_Data_Patch, disk->GetIndice().data(), static_cast<uint>(disk->GetIndice().size() * sizeof(uint)));

	// capsule
	Capsule capsule(50);
	vector<VAO::VBO_DataPatch> P3_Capsule_Vec_VBO_Data_Patch = {
		{capsule.GetPosArr(), capsule.GetPosArrSize(), 3},
		{capsule.GetNormalArr(), capsule.GetNormalArrSize(), 3},
		{capsule.GetTexCoordsArr(), capsule.GetTexCoordsArrSize(), 2},
		{capsule.GetTangentArr(), capsule.GetTangentArrSize(), 3},
	};
	VAO_P3N3T2T3_Capsule = VAO(P3_Capsule_Vec_VBO_Data_Patch, capsule.GetIndexArr(), capsule.GetIndexArrSize());
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
	case ShapeType::Screen:
		return VAO_P3T2_Screen;
	case ShapeType::Disk:
		return VAO_P3N3T2T3_Disk;
	case ShapeType::Capsule:
		return VAO_P3N3T2T3_Capsule;
	default:
		printf("ERROR::MgrVAO::GetShapeVAO:\n"
			"\t""not support shape(%s)\n", shapeType._to_string());
		return VAO::inValid;
	}
}

const VAO RawAPI_OGLW::GetVAO(PtrC<TriMesh> mesh) {
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

void RawAPI_OGLW::DirtyVAO(PtrC<TriMesh> mesh) {
	mesh2VAO.erase(mesh);
}

const Texture RawAPI_OGLW::GetTex(PtrC<Image> img) {
	auto target = img2tex.find(img);
	if (target != img2tex.end())
		return target->second;

	auto tex = Texture(img);
	img2tex[img] = tex;
	return tex;
}
