#pragma once

#include "ShapeMesh.h"

#include <array>

namespace Ubpa {
	class PlaneMesh : public ShapeMesh {
	public:
		PlaneMesh();
		float* GetNormalArr();
		float* GetTexCoordsArr();
		unsigned* GetIndexArr();
		unsigned GetNormalArrSize();
		unsigned GetTexCoordsArrSize();
		unsigned GetIndexArrSize();
	protected:
		std::vector<std::array<float, 3>> normalArr;
		std::vector<std::array<float, 2>> texCoordsArr;
		std::vector<std::array<unsigned, 3>> indexArr;
	};
}
