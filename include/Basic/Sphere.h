#ifndef _BASIC_SHAPE_SPHERE_H_
#define _BASIC_SHAPE_SPHERE_H_

#include <Basic/Shape.h>

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
			std::vector<Vec3f> normalArr;
			std::vector<Vec2f> texCoordsArr;
			std::vector<uVec3i> indexArr;
			std::vector<Vec3f> tangentArr;
		};
	}
}

#endif // !_BASIC_SHAPE_SPHERE_H_
