#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <Utility/Basic/Array2D.h>

namespace CppUtility {
	namespace Other {
		class Shape {
		public:
			Shape(size_t vertexNum, size_t triNum = 0);
			virtual ~Shape();
			float * GetVertexArr();
			size_t GetTriNum();
			size_t GetVertexArrSize();
		protected:
			static const float PI;
			//------------
			Array2D<float> * vertexArr;
			size_t triNum;
			size_t vertexNum;
		};
	}
}

#endif // !_SHAPE_H_
