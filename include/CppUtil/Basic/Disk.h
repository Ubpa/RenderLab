#ifndef _CPPUTIL_BASIC_SHAPE_DISK_H_
#define _CPPUTIL_BASIC_SHAPE_DISK_H_

#include <CppUtil/Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Disk : public Shape {
		public:
			Disk(uint n);
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


#endif // !_CPPUTIL_BASIC_SHAPE_DISK_H_
