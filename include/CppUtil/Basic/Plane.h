#ifndef _BASIC_SHAPE_PLANE_H_
#define _BASIC_SHAPE_PLANE_H_

#include <CppUtil/Basic/Shape.h>

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
			std::vector<Normalf> normalArr;
			std::vector<Point2> texCoordsArr;
			std::vector<Val<3,uint>> indexArr;
		};
	}
}

#endif//!_BASIC_SHAPE_PLANE_H_
