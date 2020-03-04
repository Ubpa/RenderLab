#ifndef _BASIC_SHAPE_SPHERE_H_
#define _BASIC_SHAPE_SPHERE_H_

#include <Basic/Shape.h>
#include <UGM/val.h>

namespace CppUtil {
	namespace Basic {
		class Sphere : public Shape {
		public:
			Sphere(unsigned n);
			float * GetNormalArr();
			float * GetTexCoordsArr();
			unsigned * GetIndexArr();
			float * GetTangentArr();
			unsigned GetNormalArrSize();
			unsigned GetTexCoordsArrSize();
			unsigned GetIndexArrSize();
			unsigned GetTangentArrSize();
		protected:
			std::vector<Ubpa::vecf3> normalArr;
			std::vector<Ubpa::vecf2> texCoordsArr;
			std::vector<Ubpa::valu3> indexArr;
			std::vector<Ubpa::vecf3> tangentArr;
		};
	}
}

#endif // !_BASIC_SHAPE_SPHERE_H_
