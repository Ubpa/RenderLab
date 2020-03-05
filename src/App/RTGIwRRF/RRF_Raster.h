#ifndef _APP_RTGI_WITH_RRF_RRF_RASTER_H_
#define _APP_RTGI_WITH_RRF_RRF_RASTER_H_

#include <Engine/ForwardRaster.h>
#include <OpenGL/FBO.h>

#include <3rdParty/enum.h>

#include <map>

namespace App {
	using namespace Ubpa;
	
	using namespace Ubpa;
	using namespace Ubpa;

	BETTER_ENUM(MODE, int, DIRECT, INDIRECT, GLOBAL);

	class RRF_Raster : public ForwardRaster {
	public:
		RRF_Raster(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera)
			: ForwardRaster(pOGLW, scene, camera), interpolateRatio(0.2f) { }
		
	public:
		static const Ptr<RRF_Raster> New(RawAPI_OGLW * pOGLW, Ptr<Scene> scene, Ptr<Camera> camera) {
			return Ubpa::New<RRF_Raster>(pOGLW, scene, camera);
		}

	protected:
		virtual ~RRF_Raster() = default;

	public:
		virtual void Draw() override;
		virtual void Init() override;

	protected:
		virtual void Visit(Ptr<SObj> sobj);

		virtual void Visit(Ptr<BSDF_FrostedGlass> bsdf);

	private:
		void InitListeners();
		void InitShaders();
		void InitShader(int ID);

	private:
		std::map<int, Shader> id2shader;
		int curID;
		float interpolateRatio;
	};
}

#endif//!_APP_RTGI_WITH_RRF_RRF_RASTER_H_
