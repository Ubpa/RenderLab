#ifndef _RAW_API_OGLW_H_
#define _RAW_API_OGLW_H_

#include <qopenglwidget.h>
#include <qopenglfunctions.h>

#include <CppUtil/Basic/Operation.h>

namespace CppUtil {
	namespace Qt {

		//class Impl_RawAPI_OGLW;

		class RawAPI_OGLW : public QOpenGLWidget, protected QOpenGLFunctions {
			Q_OBJECT
		public:
			explicit RawAPI_OGLW(QWidget* parent = nullptr, ::Qt::WindowFlags f = ::Qt::WindowFlags());
			~RawAPI_OGLW();

			void SetPaintOp(Basic::Ptr<Basic::Operation> paintOp);

		protected:
			virtual void initializeGL();
			virtual void resizeGL(int w, int h);
			virtual void paintGL();

		private:
			Basic::Ptr<Basic::Operation> paintOp;
		};
	}
}

#endif // !_RAW_API_OGLW_H_
