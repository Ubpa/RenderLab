#ifndef _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
#define _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_

#include <CppUtil/Engine/RasterBase.h>
#include <CppUtil/OpenGL/FBO.h>

namespace App {
	class SampleRaster : public CppUtil::Engine::RasterBase {
	public:
		SampleRaster(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene)
			: RasterBase(scene), haveSampled(false) { }

	public:
		static const CppUtil::Basic::Ptr<SampleRaster> New(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene) {
			return CppUtil::Basic::New<SampleRaster>(scene);
		}

	protected:
		virtual ~SampleRaster() = default;

	public:
		void Draw();
		void OGL_Init();

		enum ENUM_TYPE {
			DirectIllum,
			POSITION,
			VIEW_DIR,
			NORMAL,
			MAT_COLOR,
			IOR_ROUGHNESS_ID,
		};
		std::vector<float> GetData(ENUM_TYPE type);

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

		bool haveSampled;
	};
}

#endif//!_APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
