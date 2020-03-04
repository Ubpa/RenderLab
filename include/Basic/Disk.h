#ifndef _CPPUTIL_BASIC_SHAPE_DISK_H_
#define _CPPUTIL_BASIC_SHAPE_DISK_H_

#include <Basic/Shape.h>
#include <UGM/val.h>

namespace CppUtil {
	namespace Basic {
		class Disk : public Shape {
		public:
			Disk(unsigned n);
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


#endif // !_CPPUTIL_BASIC_SHAPE_DISK_H_
