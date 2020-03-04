#ifndef _CPPUTIL_BASIC_SAMPLER_BASIC_SAMPLER_H_
#define _CPPUTIL_BASIC_SAMPLER_BASIC_SAMPLER_H_

#include <UGM/point.h>
#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/vec.h>
#include <UGM/Normal.h>

namespace CppUtil {
	namespace Basic {
		namespace BasicSampler {
			const Ubpa::pointf2 UniformInSquare();

			// Concentric
			const Ubpa::pointf2 UniformInDisk();

			const Ubpa::pointf2 UniformOnDisk();

			//const Ubpa::pointf3 UniformInSphere();

			const Ubpa::vecf3 UniformOnSphere();
			const Ubpa::vecf3 UniformOnSphere(float & pd);
			float PDofUniformOnSphere();

			const Ubpa::vecf3 CosOnHalfSphere();

			const Ubpa::vecf3 CosOnSphere();
		}
	}
}

#endif // !_CPPUTIL_BASIC_SAMPLER_BASIC_SAMPLER_H_
