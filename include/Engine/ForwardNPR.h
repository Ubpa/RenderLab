#ifndef _CPPUTIL_ENGINE_VIEWER_FORWARD_NPR_H_
#define _CPPUTIL_ENGINE_VIEWER_FORWARD_NPR_H_

#include <Engine/ForwardRaster.h>

namespace CppUtil {
	namespace Engine {
		class ForwardNPR : public ForwardRaster {
		public:
			ForwardNPR(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera)
				: ForwardRaster(pOGLW, scene, camera) { }

		public:
			static Basic::Ptr<ForwardNPR> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera) {
				return Basic::New<ForwardNPR>(pOGLW, scene, camera);
			}

		protected:
			virtual ~ForwardNPR() = default;

		public:
			virtual void Init() override;

		protected:
			virtual void Visit(Basic::Ptr<Gooch> gooch) override;

		private:
			void InitShaders();
			void InitShaderGooch();

		private:
			OpenGL::Shader shaderGooch;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_VIEWER_FORWARD_NPR_H_
