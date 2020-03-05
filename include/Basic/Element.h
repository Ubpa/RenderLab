#pragma once

#include <Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Visitor;

		class Element : public HeapObj {
		public:
			void Accept(Ptr<Visitor> visitor);

		protected:
			Element() = default;
			virtual ~Element() = default;
		};
	}
}
