#ifndef _CPPUTIL_BASIC_SHAPE_CAPSULE_H_
#define _CPPUTIL_BASIC_SHAPE_CAPSULE_H_

#include <Basic/Shape.h>
#include <UGM/val.h>

namespace CppUtil {
	namespace Basic {
		class Capsule : public Shape {
		public:
			Capsule(unsigned n, float height = 2.f);
			float * GetNormalArr();
			float * GetTexCoordsArr();
			unsigned * GetIndexArr();
			float * GetTangentArr();
			unsigned GetNormalArrSize();
			unsigned GetTexCoordsArrSize();
			unsigned GetIndexArrSize();
			unsigned GetTangentArrSize();

			float GetHeight() const { return height; }

		protected:
			std::vector<Ubpa::vecf3> normalArr;
			std::vector<Ubpa::vecf2> texCoordsArr;
			std::vector<Ubpa::valu3> indexArr;
			std::vector<Ubpa::vecf3> tangentArr;

		private:
			float height;
		};
	}
}

#endif