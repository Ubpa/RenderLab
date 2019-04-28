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
			std::vector<std::array<float, 3>> normalArr;
			std::vector<std::array<float, 2>> texCoordsArr;
			std::vector<std::array<uint, 3>> indexArr;
			std::vector<std::array<float, 3>> tangentArr;
		};
	}
}

#endif // !_BASIC_SHAPE_SPHERE_H_
