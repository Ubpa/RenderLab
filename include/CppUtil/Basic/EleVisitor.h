#ifndef _BASIC_NODE_ELE_VISITOR_H_
#define _BASIC_NODE_ELE_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/TypeMap.h>
#include <CppUtil/Basic/GStorage.h>

#include <functional>

#define ELEVISITOR_SETUP(CLASS) \
HEAP_OBJ_SETUP(CLASS)\
private:\
/*参数不需要填写，只需给出 EleType 即可*/\
template<typename EleType>\
void Reg(EleType * useless_paramater = nullptr) {\
	void (CLASS::*visitFunc)(CppUtil::Basic::Ptr<EleType>) = &CLASS::Visit;\
	EleVisitor::Reg<CLASS, EleType>(visitFunc);\
}

#define ELEVISITOR_SETUP_FUNCNAME(CLASS, FUNCNAME) \
HEAP_OBJ_SETUP(CLASS)\
private:\
/*参数不需要填写，只需给出 EleType 即可*/\
template<typename EleType>\
void Reg(EleType * useless_paramater = nullptr) {\
	void (CLASS::*visitFunc)(CppUtil::Basic::Ptr<EleType>) = &CLASS::FUNCNAME;\
	EleVisitor::Reg<CLASS, EleType>(visitFunc);\
}

namespace CppUtil {
	namespace Basic {
		class Element;

		class EleVisitor : public HeapObj {
			HEAP_OBJ_SETUP(EleVisitor)
			
		public:
			// 静态编译期得到 typeid
			template<typename T>
			void Visit(Basic::Ptr<T> ele) {
				auto target = visitOps.find(typeid(T));
				if (target != visitOps.end())
					target->second(ele);
			}

			template<typename EleType>
			void Reg(const std::function<void (Basic::Ptr<EleType>)> & visitFunc) {
				visitOps[typeid(EleType)] = [visitFunc](Basic::Ptr<Element> pEle) {
					visitFunc(Basic::Ptr<EleType>::Cast(pEle));
				};
			}

			template<typename ArgType>
			void RegArg(const std::string & id, const ArgType & arg) {
				GS::Reg<std::string, ArgType>(std::to_string(size_t(this))+"_"+ id, arg);
			}

			template<typename ArgType>
			bool GetArg(const std::string & id, ArgType & arg) {
				return GS::GetV<std::string, ArgType>(std::to_string(size_t(this)) + "_" + id, arg);
			}

			template<typename ArgType>
			ArgType GetArg(const std::string & id, const ArgType * useless_arg = nullptr) {
				ArgType arg;
				GS::GetV<std::string, ArgType>(std::to_string(size_t(this)) + "_" + id, arg);
				return arg;
			}

		protected:
			template<typename VisitorType, typename EleType>
			void Reg(void (VisitorType::*visitFunc)(Basic::Ptr<EleType>)) {
				visitOps[typeid(EleType)] = [this, visitFunc](Basic::Ptr<Element> pEle) {
					(dynamic_cast<VisitorType*>(this)->*visitFunc)(Basic::Ptr<EleType>::Cast(pEle));
				};
			}

		private:
			TypeMap< std::function< void (Basic::Ptr<Element>) > > visitOps;
		};
	}
}

#endif // !_BASIC_NODE_ELE_VISITOR_H_
