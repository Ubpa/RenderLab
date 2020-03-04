#ifndef _CPPUTIL_BASIC_SAMPLER_BASIC_SAMPLER_H_
#define _CPPUTIL_BASIC_SAMPLER_BASIC_SAMPLER_H_

#include <Basic/UGM/Point2.h>
#include <Basic/UGM/Point3.h>
#include <Basic/UGM/Vector2.h>
#include <Basic/UGM/Vector3.h>
#include <Basic/UGM/Normal.h>

namespace CppUtil {
	namespace Basic {
		namespace BasicSampler {
			const Point2 UniformInSquare();

			// Concentric
			const Point2 UniformInDisk();

			const Point2 UniformOnDisk();

			//const Point3 UniformInSphere();

			const Vec3 UniformOnSphere();
			const Vec3 UniformOnSphere(float & pd);
			float PDofUniformOnSphere();

			const Vec3 CosOnHalfSphere();

			const Vec3 CosOnSphere();
		}
	}
}

#endif // !_CPPUTIL_BASIC_SAMPLER_BASIC_SAMPLER_H_
