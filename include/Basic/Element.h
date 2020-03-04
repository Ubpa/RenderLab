#ifndef _BASIC_NODE_ELEMENT_H_
#define _BASIC_NODE_ELEMENT_H_

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

#endif // !_BASIC_NODE_ELEMENT_H_
