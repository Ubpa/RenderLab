#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <CppUtil/Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Sphere : public Shape {
		public:
			Sphere(size_t n);
			virtual ~Sphere();
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
		};
	}
}

#endif // !_SPHERE_H_
