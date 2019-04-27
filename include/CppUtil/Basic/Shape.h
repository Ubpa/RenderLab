#ifndef _BASIC_SHAPE_SHAPE_H_
#define _BASIC_SHAPE_SHAPE_H_

typedef unsigned int uint;

#include <CppUtil/Basic/UGM/Point3.h>
#include <CppUtil/Basic/UGM/Normal.h>
#include <CppUtil/Basic/UGM/Point2.h>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Shape {
		public:
			Shape(uint vertexNum, uint triNum = 0);
			float * GetPosArr();
			uint GetVertexNum() { return vertexNum; }
			uint GetTriNum() { return triNum; }
			uint GetPosArrSize();
		protected:
			std::vector<Point3> posArr;
			uint vertexNum;
			uint triNum;
		};
	}
}

#endif // !_BASIC_SHAPE_SHAPE_H_
