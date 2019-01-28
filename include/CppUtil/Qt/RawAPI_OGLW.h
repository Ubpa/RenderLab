#ifndef _RAW_API_OGLW_H_
#define _RAW_API_OGLW_H_

#include <CppUtil/Basic/Operation.h>

#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglcontext.h>

#include <CppUtil/Basic/GStorage.h>

namespace CppUtil {
	namespace Qt {

		class RawAPI_OGLW : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
			Q_OBJECT
		public:
			explicit RawAPI_OGLW(QWidget* parent = nullptr, ::Qt::WindowFlags f = ::Qt::WindowFlags());
			~RawAPI_OGLW();

			void SetInitOp(Basic::Operation::Ptr initOp);
			void SetPaintOp(Basic::Operation::Ptr paintOp);
			void SetResizeOp(Basic::Operation::Ptr resizeOp);

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

		public:
			static std::string str_w;
			static std::string str_h;

		private:
			Basic::Operation::Ptr initOp;
			Basic::Operation::Ptr paintOp;
			Basic::Operation::Ptr resizeOp;

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
			return GS::GetV(objectName().toStdString() + uniqueID, val)
		}
	}
}

#endif // !_RAW_API_OGLW_H_
