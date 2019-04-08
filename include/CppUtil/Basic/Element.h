#ifndef _BASIC_NODE_ELEMENT_H_
#define _BASIC_NODE_ELEMENT_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/Visitor.h>

namespace CppUtil {
	namespace Basic {
		class ElementBase {
		public:
			virtual void Accept(Basic::Ptr<VisitorBase> visitor) = 0;

		protected:
			ElementBase() = default;
			virtual ~ElementBase() = default;
		};

		template<typename ImplT>
		class Element : public ElementBase, public HeapObj<ImplT> {
		protected:
			Element() = default;
			virtual ~Element() = default;

		public:
			virtual void Accept(Basic::Ptr<VisitorBase> visitor) override {
				visitor->Visit(This());
			}
		};
	}
}

#endif // !_BASIC_NODE_ELEMENT_H_
