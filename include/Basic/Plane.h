#ifndef _BASIC_SHAPE_PLANE_H_
#define _BASIC_SHAPE_PLANE_H_

#include <Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Plane : public Shape {
		public: 
			Plane();
			float * GetNormalArr();
			float * GetTexCoordsArr();
			uint * GetIndexArr();
			uint GetNormalArrSize();
			uint GetTexCoordsArrSize();
			uint GetIndexArrSize();
		protected:
			std::vector<std::array<float, 3>> normalArr;
			std::vector<std::array<float, 2>> texCoordsArr;
			std::vector<std::array<uint, 3>> indexArr;
		};
	}
}

#endif//!_BASIC_SHAPE_PLANE_H_
