#ifndef _CUBE_H_
#define _CUBE_H_

#include <Utility/Basic/Shape.h>

namespace CppUtility {
	namespace Other {
		class Cube : public Shape {
		public:
			Cube();
			virtual ~Cube();
			float * GetNormalArr();
			float * GetTexCoordsArr();
			size_t * GetIndexArr();
			size_t GetNormalArrSize();
			size_t GetTexCoordsArrSize();
			size_t GetIndexArrSize();
		protected:
			Array2D<float> * normalArr;
			Array2D<float> * texCoordsArr;
			Array2D<size_t> * indexArr;
			static const float cubeData[192];
		};
	}
}

#endif // !_CUBE_H_
