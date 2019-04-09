#ifndef _BASIC_NODE_VISITOR_H_
#define _BASIC_NODE_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/TypeMap.h>
#include <CppUtil/Basic/GStorage.h>

#include <CppUtil/Basic/FunctionTraits.h>

#include <functional>

namespace CppUtil {
	namespace Basic {
		class Visitor : public HeapObj {
		public:
			static const Ptr<Visitor> New() { return Basic::New<Visitor>(); }

		protected:
			virtual ~Visitor() = default;

		private:// Visit 函数只能让 Element 调用
			friend class Element;

			// 动态
			void Visit(Ptr<Element> ele);

		public:
			template<typename LambadaExpr>
			void Reg(LambadaExpr lambdaVisitFunc) {
				using ptrE = FunctionTraitsLambda<LambadaExpr>::arg<0>::type;
				using EleType = ptrE::element_type;
				const Func func = [lambdaVisitFunc](Ptr<Element> pEle) {
					lambdaVisitFunc(CastTo<EleType>(pEle));
				};
				visitOps[typeid(EleType)] = func;
			}

		protected:
			template<typename EleType, typename ImplT>
			void RegMemberFunc(void (ImplT::*visitFunc)(Ptr<EleType>)) {
				ImplT * obj = dynamic_cast<ImplT*>(this);
				visitOps[typeid(EleType)] = [obj, visitFunc](Ptr<Element> pEle) {
					(obj->*visitFunc)(CastTo<EleType>(pEle));
				};
			}

		private:
			using Func = std::function< void(Ptr<Element>) >;
			TypeMap< std::function< void(Ptr<Element>) > > visitOps;
		};
	}
}

#endif // !_BASIC_NODE_VISITOR_H_
