#ifndef _Q_OPENGL_API_H_
#define _Q_OPENGL_API_H_

#include <CppUtil/Basic/Singleton.h>
#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_compatibility.h>

#define QOGL CppUtil::Qt::QOpenGL_API::GetInstance()

namespace CppUtil {
	namespace Qt {
		class QOpenGL_API : protected QOpenGLWidget, public QOpenGLFunctions_3_3_Compatibility {
		public:
			static QOpenGL_API * GetInstance() {
				static QOpenGL_API * instance = new QOpenGL_API();
				return instance;
			}
			QOpenGLContext * GetContext();
		protected:
			QOpenGL_API();
		};
	}
}

#endif // !_Q_OPENGL_API_H_
