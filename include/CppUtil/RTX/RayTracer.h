#ifndef _RAY_TRACER_H_
#define _RAY_TRACER_H_

#include <CppUtil/Basic/Ptr.h>
#include <glm/glm.hpp>

namespace RTX {
	class Hitable;
	class Ray;

	class RayTracer {
	public:
		RayTracer(CppUtil::Basic::CPtr<Hitable> scene);
		
		glm::vec3 Trace(CppUtil::Basic::Ptr<Ray> & ray, int depth = 0);

	private:
		CppUtil::Basic::CPtr<Hitable> scene;
	};
}

#endif // !_RAY_TRACER_H_
