#ifndef _RAW_API_OGLW_H_
#define _RAW_API_OGLW_H_

#include <CppUtil/Basic/Operation.h>

#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_core.h>
#include <qopenglcontext.h>

namespace CppUtil {
	namespace Qt {

		class RawAPI_OGLW : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
			Q_OBJECT
		public:
			explicit RawAPI_OGLW(QWidget* parent = nullptr, ::Qt::WindowFlags f = ::Qt::WindowFlags());
			~RawAPI_OGLW();

			void SetPaintOp(Basic::Operation::Ptr paintOp);
			void SetInitOp(Basic::Operation::Ptr initOp);

		protected:
			virtual void initializeGL();
			virtual void resizeGL(int w, int h);
			virtual void paintGL();

		private:
			Basic::Operation::Ptr initOp;
			Basic::Operation::Ptr paintOp;
			size_t ID;
		};
	}
}

#endif // !_RAW_API_OGLW_H_
