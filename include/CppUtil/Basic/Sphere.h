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
			float * GetTangentArr();
			uint GetNormalArrSize();
			uint GetTexCoordsArrSize();
			uint GetIndexArrSize();
			uint GetTangentArrSize();
		protected:
			Array2D<float> * normalArr;
			Array2D<float> * texCoordsArr;
			Array2D<uint> * indexArr;
			Array2D<float> * tangentArr;
		};
	}
}

#endif // !_BASIC_SHAPE_SPHERE_H_
