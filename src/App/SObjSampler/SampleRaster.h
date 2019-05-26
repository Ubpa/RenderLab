#ifndef _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
#define _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_

#include <CppUtil/Engine/ForwardRaster.h>
#include <CppUtil/OpenGL/FBO.h>

namespace App {
	using namespace CppUtil;
	using namespace CppUtil::Basic;
	using namespace CppUtil::Engine;
	using namespace CppUtil::OpenGL;
	using namespace CppUtil::QT;

	class SampleRaster : public ForwardRaster {
	public:
		SampleRaster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
			: ForwardRaster(pOGLW, scene, camera), haveSampled(false) { }

	public:
		static const Ptr<SampleRaster> New(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Basic::New<SampleRaster>(pOGLW, scene, camera);
		}

	protected:
		virtual ~SampleRaster() = default;

	public:
		void Draw();
		void Init();

		enum class ENUM_TYPE {
			DirectIllum,
			POSITION,
			VIEW_DIR,
			NORMAL,
			MAT_COLOR,
			IOR_ROUGHNESS_ID,
		};
		std::vector<float> GetData(ENUM_TYPE type);

	protected:
		virtual void Visit(Ptr<SObj> sobj);

		virtual void Visit(Ptr<BSDF_FrostedGlass> bsdf);

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
