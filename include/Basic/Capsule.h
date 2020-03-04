#ifndef _CPPUTIL_BASIC_SHAPE_CAPSULE_H_
#define _CPPUTIL_BASIC_SHAPE_CAPSULE_H_

#include <Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Capsule : public Shape {
		public:
			Capsule(uint n, float height = 2.f);
			float * GetNormalArr();
			float * GetTexCoordsArr();
			uint * GetIndexArr();
			float * GetTangentArr();
			uint GetNormalArrSize();
			uint GetTexCoordsArrSize();
			uint GetIndexArrSize();
			uint GetTangentArrSize();

			float GetHeight() const { return height; }

		protected:
			std::vector<Vec3f> normalArr;
			std::vector<Vec2f> texCoordsArr;
			std::vector<uVec3i> indexArr;
			std::vector<Vec3f> tangentArr;

		private:
			float height;
		};
	}
}

#endif