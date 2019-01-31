#ifndef _INTERSECTOR_H_
#define _INTERSECTOR_H_

#include <CppUtil/Basic/EleVisitor.h>

namespace CppUtil {
	namespace Engine {
		// list out all primitives
		class Sphere;

		class Intersector : public Basic::EleVisitor {
			HEAP_OBJ_SETUP(Intersector)
		public:
			struct Rst {
				Rst(bool isIntersect = false)
					: isIntersect(false) { }

				void Clear() { isIntersect = false; }
				bool isIntersect;
			};

		public:
			virtual void Intersect(Basic::CPtr<Sphere> sphere) = 0;

			virtual const Rst * GetRst() const = 0;

		protected:
			template<typename ChildT>
			void Reg(ChildT * useless_parameter = nullptr) {
				Basic::EleVisitor::Reg<ChildT, const Sphere>(&ChildT::Intersect);
			}

			template<typename ChildT>
			void UnReg(ChildT * useless_parameter = nullptr) {
				Basic::EleVisitor::UnReg<ChildT, const Sphere>(&ChildT::Intersect);
			}

		private:
			using Basic::EleVisitor::Reg;
			using Basic::EleVisitor::UnReg;
		};
	}
}

#endif//!_INTERSECTOR_H_