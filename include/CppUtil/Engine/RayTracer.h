#ifndef _ENGINE_RTX_RAY_TRACER_H_
#define _ENGINE_RTX_RAY_TRACER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/UGM/RGB.h>

namespace CppUtil {
	namespace Engine {
		class Scene;

		class RayTracerBase : public Basic::HeapObjBase {
		protected:
			RayTracerBase() = default;
			virtual ~RayTracerBase() = default;

		public:
			// ray 处于世界坐标系
			virtual const RGBf Trace(Ray & ray) = 0;
			virtual void Init(Basic::Ptr<Scene> scene) {}
		};

		template<typename ImplT, typename BaseT = RayTracerBase>
		class RayTracer : public Basic::HeapObj<ImplT, BaseT> {
		protected:
			using HeapObj<ImplT, BaseT>::HeapObj;
			virtual ~RayTracer() = default;
		};
	}
}

#endif//!_ENGINE_RTX_RAY_TRACER_H_
