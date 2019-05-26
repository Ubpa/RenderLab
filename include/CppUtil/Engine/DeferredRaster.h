#ifndef _CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_
#define _CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_

#include <CppUtil/Engine/Raster.h>

#include <CppUtil/OpenGL/FBO.h>
#include <CppUtil/Basic/UGM/Transform.h>

namespace CppUtil{
	namespace Engine {
		class SObj;

		class Sphere;
		class Plane;
		class TriMesh;

		class BSDF_MetalWorkflow;

		/*
		�ӳٹ���
		��ʱֻ���ڽ���������������������ʱ��������һ�������Ļ���
		*/
		class DeferredRaster : public Raster {
		public:
			DeferredRaster(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera);

		public:
			static const Basic::Ptr<DeferredRaster> New(QT::RawAPI_OGLW * pOGLW, Basic::Ptr<Scene> scene, Basic::Ptr<OpenGL::Camera> camera) {
				return Basic::New<DeferredRaster>(pOGLW, scene, camera);
			}

		public:
			virtual void Init() override;
			virtual void Resize() override;
			virtual void Draw() override;

		private:
			virtual void Visit(Basic::Ptr<SObj> sobj);

			virtual void Visit(Basic::Ptr<BSDF_MetalWorkflow> bsdf);

			virtual void Visit(Basic::Ptr<Sphere> sphere);
			virtual void Visit(Basic::Ptr<Plane> plane);
			virtual void Visit(Basic::Ptr<TriMesh> mesh);

		private:
			void Pass_GBuffer();

		private:
			OpenGL::FBO gbufferFBO;
			std::vector<Transform> modelVec;
			OpenGL::Shader metalShader;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_VIEWER_DEFFERRED_RASTER_H_
