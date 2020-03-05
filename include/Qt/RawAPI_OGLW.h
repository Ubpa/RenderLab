#pragma once

#include <OpenGL/VAO.h>
#include <OpenGL/Texture.h>

#include <Basic/GStorage.h>
#include <Basic/Ptr.h>

#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglcontext.h>

#include <map>

namespace Ubpa {
	class Op;
	class OpQueue;
	class Image;
	class TriMesh;

	class RawAPI_OGLW : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
		Q_OBJECT
	public:
		explicit RawAPI_OGLW(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
		~RawAPI_OGLW();

		enum class ShapeType {
			Sphere,
			Plane,
			Cube,
			Screen,
			Disk,
			Capsule
		};

	public:
		const VAO GetVAO(RawAPI_OGLW::ShapeType shapeType);
		const VAO GetVAO(PtrC<TriMesh> mesh);
		void DirtyVAO(PtrC<TriMesh> mesh);
		const Texture GetTex(PtrC<Image> img);

		template<typename T>
		bool Reg(const std::string& uniqueID, const T& item);
		template<typename T>
		void GetP(const std::string& uniqueID, T*& p);
		template<typename T>
		bool GetV(const std::string& uniqueID, T& val);

	public:
		void AddInitOp(Ptr<Op> initOp);
		void AddPaintOp(Ptr<Op> paintOp);
		void AddResizeOp(Ptr<Op> resizeOp);

	protected:
		virtual void initializeGL();
		virtual void paintGL();
		virtual void resizeGL(int w, int h);

	protected:
		virtual void keyPressEvent(QKeyEvent* event);
		virtual void keyReleaseEvent(QKeyEvent* event);
		virtual void mouseMoveEvent(QMouseEvent* event);
		virtual void mousePressEvent(QMouseEvent* event);
		virtual void mouseReleaseEvent(QMouseEvent* event);
		virtual void wheelEvent(QWheelEvent* event);

	private:
		void InitShapeVAOs();

	public:
		int w;
		int h;
		int x;
		int y;
		int angle;

	private:
		Ptr<OpQueue> initOpQueue;
		Ptr<OpQueue> paintOpQueue;
		Ptr<OpQueue> resizeOpQueue;

		VAO VAO_P3N3T2T3_Sphere;
		VAO VAO_P3N3T2T3_Plane;
		VAO VAO_P3T2_Screen;
		VAO VAO_P3N3T2T3_Disk;
		VAO VAO_P3N3T2T3_Capsule; // height is 2
		std::map<WPtrC<TriMesh>, VAO> mesh2VAO;
		std::map<WPtrC<Image>, Texture> img2tex;

		size_t ID;

		static const PtrC<TriMesh> cube;
	};

	template<typename T>
	bool RawAPI_OGLW::Reg(const std::string& uniqueID, const T& item) {
		makeCurrent();
		return GS::Reg(objectName().toStdString() + uniqueID, item);
	}

	template<typename T>
	void RawAPI_OGLW::GetP(const std::string& uniqueID, T*& p) {
		GS::GetP(objectName().toStdString() + uniqueID, p);
	}

	template<typename T>
	bool RawAPI_OGLW::GetV(const std::string& uniqueID, T& val) {
		return GS::GetV(objectName().toStdString() + uniqueID, val);
	}
}
