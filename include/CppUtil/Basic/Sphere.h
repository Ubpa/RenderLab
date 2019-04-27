#ifndef _BASIC_SHAPE_SPHERE_H_
#define _BASIC_SHAPE_SPHERE_H_

#include <CppUtil/Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Sphere : public Shape {
		public:
			Sphere(uint n);
			float * GetNormalArr();
			float * GetTexCoordsArr();
			uint * GetIndexArr();
			float * GetTangentArr();
			uint GetNormalArrSize();
			uint GetTexCoordsArrSize();
			uint GetIndexArrSize();
			uint GetTangentArrSize();
		protected:
			std::vector<Normalf> normalArr;
			std::vector<Point2> texCoordsArr;
			std::vector<Val<3, uint>> indexArr;
			std::vector<Normalf> tangentArr;
		};
	}
}

#endif // !_BASIC_SHAPE_SPHERE_H_
