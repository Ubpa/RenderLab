#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <CppUtil/Basic/Array2D.h>

namespace CppUtil {
	namespace Basic {
		class Shape {
		public:
			Shape(size_t vertexNum, size_t triNum = 0);
			virtual ~Shape();
			float * GetPosArr();
			size_t GetVertexNum() { return vertexNum; }
			size_t GetTriNum() { return triNum; }
			size_t GetPosArrSize();
		protected:
			static const float PI;
			//------------
			Array2D<float> * posArr;
			size_t vertexNum;
			size_t triNum;
		};
	}
}

#endif // !_SHAPE_H_
