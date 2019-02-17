#ifndef _BASIC_SHAPE_SHAPE_H_
#define _BASIC_SHAPE_SHAPE_H_

typedef unsigned int uint;

#include <CppUtil/Basic/Array2D.h>

namespace CppUtil {
	namespace Basic {
		class Shape {
		public:
			Shape(uint vertexNum, uint triNum = 0);
			virtual ~Shape();
			float * GetPosArr();
			uint GetVertexNum() { return vertexNum; }
			uint GetTriNum() { return triNum; }
			uint GetPosArrSize();
		protected:
			static const float PI;
			//------------
			Array2D<float> * posArr;
			uint vertexNum;
			uint triNum;
		};
	}
}

#endif // !_BASIC_SHAPE_SHAPE_H_
