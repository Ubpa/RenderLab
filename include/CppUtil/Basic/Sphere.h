#ifndef _BASIC_SHAPE_SPHERE_H_
#define _BASIC_SHAPE_SPHERE_H_

#include <CppUtil/Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Sphere : public Shape {
		public:
			Sphere(uint n);
			virtual ~Sphere();
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
		};
	}
}

#endif // !_BASIC_SHAPE_SPHERE_H_
