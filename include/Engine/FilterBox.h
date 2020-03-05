#pragma once

#include <Engine/Filter.h>

namespace CppUtil {
	namespace Engine {
		class FilterBox : public Filter {
		public:
			FilterBox(const Ubpa::vecf2 & radius) : Filter(radius) { }

		protected:
			virtual ~FilterBox() = default;

		public:
			const Basic::Ptr<FilterBox> New(const Ubpa::vecf2 & radius) {
				return Basic::New<FilterBox>(radius);
			}

		public:
			virtual float Evaluate(const Ubpa::pointf2 & p) const override {
				return 1;
			}
		};
	}
}
