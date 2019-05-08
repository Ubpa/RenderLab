#ifndef _APP_RTGI_WITH_RRF_RRF_RASTER_H_
#define _APP_RTGI_WITH_RRF_RRF_RASTER_H_

#include <CppUtil/Engine/RasterBase.h>
#include <CppUtil/OpenGL/FBO.h>

#include <3rdParty/enum.h>

#include <map>

namespace App {
	BETTER_ENUM(MODE, int,
		DIRECT,
		INDIRECT,
		GLOBAL)

	class RRF_Raster : public CppUtil::Engine::RasterBase {
	public:
		RRF_Raster(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene)
			: RasterBase(scene), interpolateRatio(0.2f) { }
		
	public:
		static const CppUtil::Basic::Ptr<RRF_Raster> New(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene) {
			return CppUtil::Basic::New<RRF_Raster>(scene);
		}

	protected:
		virtual ~RRF_Raster() = default;

	public:
		void Draw();
		void OGL_Init();

	protected:
		virtual void Visit(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

		virtual void Visit(CppUtil::Basic::Ptr<CppUtil::Engine::BSDF_FrostedGlass> bsdf);

	private:
		void InitListeners();
		void InitShaders();
		void InitShader(int ID);

	private:
		std::map<int, CppUtil::OpenGL::Shader> id2shader;
		int curID;
		float interpolateRatio;
	};
}

#endif//!_APP_RTGI_WITH_RRF_RRF_RASTER_H_
