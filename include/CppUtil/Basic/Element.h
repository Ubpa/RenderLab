#ifndef _BASIC_NODE_ELEMENT_H_
#define _BASIC_NODE_ELEMENT_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/Visitor.h>

namespace CppUtil {
	namespace Basic {
		class ElementBase : public HeapObjBase {
		public:
			virtual void Accept(Ptr<Visitor> visitor) = 0;

		protected:
			ElementBase() = default;
			virtual ~ElementBase() = default;
		};

		// ImplT 需要一直传递到最终的类，然后用 final
		template<typename ImplT, typename BaseT = ElementBase>
		class Element : public HeapObj<ImplT, BaseT> {
		protected:
			using HeapObj<ImplT, BaseT>::HeapObj;
			virtual ~Element() = default;

		public:
			virtual void Accept(Ptr<Visitor> visitor) override {
				visitor->Visit(This());
			}
		};
	}
}

#endif // !_BASIC_NODE_ELEMENT_H_
