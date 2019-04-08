#ifndef _ENGINE_INTERSECTOR_INTERSECTOR_H_
#define _ENGINE_INTERSECTOR_INTERSECTOR_H_

#include <CppUtil/Basic/Visitor.h>

namespace CppUtil {
	namespace Engine {
		class IntersectorBase : public Basic::VisitorBase {
		protected:
			IntersectorBase() = default;
			virtual ~IntersectorBase() = default;

		public:
			struct Rst {
				Rst(bool isIntersect = false)
					: isIntersect(false) { }

				bool IsIntersect() const { return isIntersect; }

			protected:
				bool isIntersect;
			};
		};

		template<typename ImplT, typename BaseT = IntersectorBase>
		class Intersector : public Basic::Visitor<ImplT, BaseT> {
		protected:
			using Visitor<ImplT, BaseT>::Visitor;
			virtual ~Intersector() = default;
		};
	}
}

#endif//!_ENGINE_INTERSECTOR_INTERSECTOR_H_