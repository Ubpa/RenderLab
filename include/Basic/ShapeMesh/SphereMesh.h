#pragma once

#include "ShapeMesh.h"

#include <UGM/val.h>

namespace Ubpa {
	class SphereMesh : public ShapeMesh {
	public:
		SphereMesh(unsigned n);
		float* GetNormalArr();
		float* GetTexCoordsArr();
		unsigned* GetIndexArr();
		float* GetTangentArr();
		unsigned GetNormalArrSize();
		unsigned GetTexCoordsArrSize();
		unsigned GetIndexArrSize();
		unsigned GetTangentArrSize();
	protected:
		std::vector<vecf3> normalArr;
		std::vector<vecf2> texCoordsArr;
		std::vector<valu3> indexArr;
		std::vector<vecf3> tangentArr;
	};
}
