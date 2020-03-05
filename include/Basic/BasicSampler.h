#pragma once

#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/normal.h>

namespace Ubpa {
	namespace BasicSampler {
		const pointf2 UniformInSquare();

		// Concentric
		const pointf2 UniformInDisk();

		const pointf2 UniformOnDisk();

		//const pointf3 UniformInSphereMesh();

		const vecf3 UniformOnSphere();
		const vecf3 UniformOnSphere(float& pd);
		float PDofUniformOnSphere();

		const vecf3 CosOnHalfSphereMesh();

		const vecf3 CosOnSphereMesh();
	}
}
