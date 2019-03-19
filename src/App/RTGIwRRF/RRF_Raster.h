#ifndef _APP_RTGI_WITH_RRF_RRF_RASTER_H_
#define _APP_RTGI_WITH_RRF_RRF_RASTER_H_

#include <CppUtil/Engine/RasterBase.h>
#include <CppUtil/OpenGL/FBO.h>

#include <map>

namespace App {
	class RRF_Raster : public CppUtil::Engine::RasterBase {
		ELEVISITOR_SETUP(RRF_Raster)
	public:
		RRF_Raster(CppUtil::Basic::Ptr<CppUtil::Engine::Scene> scene)
			: RasterBase(scene) { }

		void Draw();
		void Init();

	protected:
		virtual void Visit(CppUtil::Basic::Ptr<CppUtil::Engine::SObj> sobj);

		virtual void Visit(CppUtil::Basic::Ptr<CppUtil::Engine::BSDF_FrostedGlass> bsdf);

	private:
		void InitShaders();
		void InitShader(int ID);

	private:
		std::map<int, CppUtil::OpenGL::Shader> id2shader;
		int curID;
	};
}

#endif//!_APP_RTGI_WITH_RRF_RRF_RASTER_H_
