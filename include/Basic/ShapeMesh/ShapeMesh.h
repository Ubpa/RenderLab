#pragma once

#include <UGM/vec.h>

#include <vector>

namespace Ubpa {
	class ShapeMesh {
	public:
		ShapeMesh(unsigned vertexNum, unsigned triNum = 0);
		float* GetPosArr();
		unsigned GetVertexNum() { return vertexNum; }
		unsigned GetTriNum() { return triNum; }
		unsigned GetPosArrSize();
	protected:
		std::vector<vecf3> posArr;
		unsigned vertexNum;
		unsigned triNum;
	};
}
