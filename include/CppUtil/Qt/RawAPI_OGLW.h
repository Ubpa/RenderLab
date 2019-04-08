#ifndef _QT_RAW_API_OGLW_RAW_API_OGLW_H_
#define _QT_RAW_API_OGLW_RAW_API_OGLW_H_

#include <CppUtil/Basic/GStorage.h>
#include <CppUtil/Basic/Ptr.h>

#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglcontext.h>

namespace CppUtil {
	namespace Basic {
		class Op;
	}

	namespace QT {

		class RawAPI_OGLW : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
			Q_OBJECT
		public:
			explicit RawAPI_OGLW(QWidget* parent = nullptr,Qt::WindowFlags f =Qt::WindowFlags());
			~RawAPI_OGLW();

			void SetInitOp(Basic::Ptr<Basic::Op> initOp);
			void SetPaintOp(Basic::Ptr<Basic::Op> paintOp);
			void SetResizeOp(Basic::Ptr<Basic::Op> resizeOp);

			template<typename T>
			bool Reg(const std::string & uniqueID, const T & item);
			template<typename T>
			void GetP(const std::string & uniqueID, T * & p);
			template<typename T>
			bool GetV(const std::string & uniqueID, T & val);

		protected:
			virtual void initializeGL();
			virtual void paintGL();
			virtual void resizeGL(int w, int h);

		protected:
			virtual void keyPressEvent(QKeyEvent *event);
			virtual void keyReleaseEvent(QKeyEvent *event);
			virtual void mouseMoveEvent(QMouseEvent *event);
			virtual void mousePressEvent(QMouseEvent *event);
			virtual void mouseReleaseEvent(QMouseEvent *event);
			virtual void wheelEvent(QWheelEvent *event);

		public:
			int w;
			int h;
			int x;
			int y;
			int angle;

		private:
			Basic::Ptr<Basic::Op> initOp;
			Basic::Ptr<Basic::Op> paintOp;
			Basic::Ptr<Basic::Op> resizeOp;

			size_t ID;
		};


		template<typename T>
		bool RawAPI_OGLW::Reg(const std::string & uniqueID, const T & item) {
			makeCurrent();
			return CppUtil::Basic::GS::Reg(objectName().toStdString() + uniqueID, item);
		}

		template<typename T>
		void RawAPI_OGLW::GetP(const std::string & uniqueID, T * & p) {
			GS::GetP(objectName().toStdString() + uniqueID, p);
		}

		template<typename T>
		bool RawAPI_OGLW::GetV(const std::string & uniqueID, T & val) {
			return GS::GetV(objectName().toStdString() + uniqueID, val);
		}
	}
}

#endif // !_QT_RAW_API_OGLW_RAW_API_OGLW_H_
