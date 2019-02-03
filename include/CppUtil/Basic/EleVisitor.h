#ifndef _ELE_VISITOR_H_
#define _ELE_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/TypeMap.h>

#include <functional>
#include <vector>

#define ELEVISITOR_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
private:\
/*参数不需要填写，只需给出 EleType 即可*/\
template<typename EleType>\
void Reg(EleType * useless_paramater = nullptr) {\
	bool (CLASS::*visitFunc)(CppUtil::Basic::Ptr<EleType>) = &CLASS::Visit;\
	EleVisitor::Reg<CLASS, EleType>(visitFunc);\
}

namespace CppUtil {
	namespace Basic {
		class Element;

		class EleVisitor : public HeapObj {
			HEAP_OBJ_SETUP(EleVisitor)
			
		public:
			// 静态编译期得到 typeid
			// 在遍历的时候，返回 true 表示继续遍历
			template<typename T>
			bool Visit(Basic::Ptr<T> ele) {
				auto target = visitOps.find(typeid(T));
				if (target == visitOps.end())
					return false;

				return target->second(ele);
			}

			template<typename EleType>
			void Reg(std::function<bool(Basic::Ptr<EleType>)> visitFunc) {
				visitOps[typeid(EleType)] = [visitFunc](Basic::Ptr<Element> pEle)->bool {
					return visitFunc(Basic::Ptr<EleType>(pEle));
				};
			}

		protected:
			template<typename VisitorType, typename EleType>
			void Reg(bool (VisitorType::*visitFunc)(Basic::Ptr<EleType>)) {
				visitOps[typeid(EleType)] = [this, visitFunc](Basic::Ptr<Element> pEle)->bool {
					return (dynamic_cast<VisitorType*>(this)->*visitFunc)(Basic::Ptr<EleType>(pEle));
				};
			}

		private:
			TypeMap< std::function< bool (Basic::Ptr<Element>) > > visitOps;
		};
	}
}

#endif // !_NODE_VISITOR_H_
