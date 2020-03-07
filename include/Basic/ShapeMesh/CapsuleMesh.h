#pragma once

#include "ShapeMesh.h"

#include <UGM/val.h>

namespace Ubpa {
	class CapsuleMesh : public ShapeMesh {
	public:
		CapsuleMesh(unsigned n, float height = 2.f);
		float* GetNormalArr();
		float* GetTexCoordsArr();
		unsigned* GetIndexArr();
		float* GetTangentArr();
		unsigned GetNormalArrSize();
		unsigned GetTexCoordsArrSize();
		unsigned GetIndexArrSize();
		unsigned GetTangentArrSize();

		float GetHeight() const { return height; }

	protected:
		std::vector<vecf3> normalArr;
		std::vector<vecf2> texCoordsArr;
		std::vector<valu3> indexArr;
		std::vector<vecf3> tangentArr;

	private:
		float height;
	};
}
