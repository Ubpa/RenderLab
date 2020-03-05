#pragma once

#include <Basic/Shape.h>

namespace CppUtil {
	namespace Basic {
		class Plane : public Shape {
		public: 
			Plane();
			float * GetNormalArr();
			float * GetTexCoordsArr();
			unsigned * GetIndexArr();
			unsigned GetNormalArrSize();
			unsigned GetTexCoordsArrSize();
			unsigned GetIndexArrSize();
		protected:
			std::vector<std::array<float, 3>> normalArr;
			std::vector<std::array<float, 2>> texCoordsArr;
			std::vector<std::array<unsigned, 3>> indexArr;
		};
	}
}
