#ifndef _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
#define _APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_

#include <Engine/ForwardRaster.h>
#include <OpenGL/FBO.h>

namespace App {
	using namespace Ubpa;
	
	using namespace Ubpa;
	
	using namespace Ubpa;

	class SampleRaster : public ForwardRaster {
	public:
		SampleRaster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
			: ForwardRaster(pOGLW, scene, camera), haveSampled(false) { }

	public:
		static const Ptr<SampleRaster> New(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Ubpa::New<SampleRaster>(pOGLW, scene, camera);
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
		VAO screen;
		FBO gBuffer;

		Shader shader_screen;
		Shader shader_sampleFrostedGlass;

		bool haveSampled;
	};
}

#endif//!_APP_SOBJ_SAMPLER_SAMPLE_RASTER_H_
