#ifndef _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
#define _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_

#include <CppUtil/Engine/RasterBase.h>
#include <CppUtil/OpenGL/FBO.h>

namespace App {
	class SampleRaster : public CppUtil::Engine::RasterBase {
		ELEVISITOR_SETUP(SampleRaster)
	public:
		SampleRaster(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene) : RasterBase(scene) { }

		void Draw();
		void Init();

	protected:
		virtual void Visit(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

		virtual void Visit(CppUtil::Basic::Ptr<CppUtil::Engine::BSDF_FrostedGlass> bsdf);

	private:
		void InitShaders();
		void InitShaderSampleFrostedGlass();
		void InitShaderScreen();

	private:
		CppUtil::OpenGL::VAO screen;
		CppUtil::OpenGL::FBO gBuffer;

		CppUtil::OpenGL::Shader shader_screen;
		CppUtil::OpenGL::Shader shader_sampleFrostedGlass;
	};
}

#endif//!_APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
