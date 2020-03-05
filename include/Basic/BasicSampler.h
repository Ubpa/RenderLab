#pragma once

#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>

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
