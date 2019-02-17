#ifndef _BASIC_SHAPE_CUBE_H_
#define _BASIC_SHAPE_CUBE_H_

#include <CppUtil/Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Cube : public Shape {
		public:
			Cube();
			virtual ~Cube();
			float * GetNormalArr();
			float * GetTexCoordsArr();
			uint * GetIndexArr();
			uint GetNormalArrSize();
			uint GetTexCoordsArrSize();
			uint GetIndexArrSize();
		protected:
			Array2D<float> * normalArr;
			Array2D<float> * texCoordsArr;
			Array2D<uint> * indexArr;
			static const float cubeData[192];
		};
	}
}

#endif // !_BASIC_SHAPE_CUBE_H_
