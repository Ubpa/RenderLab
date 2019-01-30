#ifndef _ELE_VISITOR_H_
#define _ELE_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>
#include <CppUtil/Basic/LStorage.h>

#include <functional>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Element;

		class EleVisitor : public HeapObj {
			HEAP_OBJ_SETUP(EleVisitor)
		public:
			template<typename T>
			void Visit(Basic::Ptr<T> ele) {
				Visit_Reg_UnReg<T>(ele);
			}
		protected:
			template<typename T, typename ChildType>
			void Reg(void (ChildType::* visitFunc)(Basic::Ptr<T>)) {
				Visit_Reg_UnReg<T>(nullptr, [this, visitFunc](Basic::Ptr<T> a) { (dynamic_cast<ChildType*>(this)->*visitFunc)(a); });
			}

			/* 为了对称，不使用这个接口
			template<typename T>
			void UnReg(Basic::Ptr<T> * useless_parameter = nullptr) {
				Visit_Reg_UnReg<T>(nullptr, nullptr);
			}
			*/

			template<typename T, typename ChildType>
			void UnReg(void (ChildType::* visitFunc)(Basic::Ptr<T>)) {
				Visit_Reg_UnReg<T>(nullptr, nullptr);
			}

		private:
			template<typename T>
			void Visit_Reg_UnReg(Basic::Ptr<T> ele = nullptr, std::function<void(Basic::Ptr<T>)> regOp = nullptr) {
				static LStorage<EleVisitor*, std::function<void(Basic::Ptr<T>)>> lsT;

				if (ele != nullptr) {
					std::function<void(Basic::Ptr<T>)> op = nullptr;
					lsT.GetV(this, op);
					if (op == nullptr)
						return;

					op(ele);
				}
				else if (regOp != nullptr)
					lsT.Reg(this, regOp);
				else// ele == nullptr && regOp == nullptr
					lsT.UnReg(this);
			}
		};
	}
}

#endif // !_NODE_VISITOR_H_
