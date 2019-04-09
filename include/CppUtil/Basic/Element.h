#ifndef _BASIC_NODE_ELEMENT_H_
#define _BASIC_NODE_ELEMENT_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Visitor;

		class Element : public HeapObj {
		public:
			// 可以传入更具体的类型来加速
			// 如 Accept<Sphere>(visitor);
			void Accept(Ptr<Visitor> visitor);

		protected:
			Element() = default;
			virtual ~Element() = default;
		};
	}
}

#endif // !_BASIC_NODE_ELEMENT_H_
