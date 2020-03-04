#ifndef _BASIC_SHAPE_SHAPE_H_
#define _BASIC_SHAPE_SHAPE_H_

#include <UGM/vec.h>

#include <vector>
#include <array>

namespace CppUtil {
	namespace Basic {
		class Shape {
		public:
			Shape(unsigned vertexNum, unsigned triNum = 0);
			float * GetPosArr();
			unsigned GetVertexNum() { return vertexNum; }
			unsigned GetTriNum() { return triNum; }
			unsigned GetPosArrSize();
		protected:
			std::vector<Ubpa::vecf3> posArr;
			unsigned vertexNum;
			unsigned triNum;
		};
	}
}

#endif // !_BASIC_SHAPE_SHAPE_H_
