#ifndef _BASIC_SHAPE_CUBE_H_
#define _BASIC_SHAPE_CUBE_H_

#include <CppUtil/Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Cube : public Shape {
		public:
			Cube();
			float * GetNormalArr();
			float * GetTexCoordsArr();
			uint * GetIndexArr();
			uint GetNormalArrSize();
			uint GetTexCoordsArrSize();
			uint GetIndexArrSize();
		protected:
			std::vector<Normalf> normalArr;
			std::vector<Point2> texCoordsArr;
			std::vector<Val<3, uint>> indexArr;
			static const float cubeData[192];
		};
	}
}

#endif // !_BASIC_SHAPE_CUBE_H_
