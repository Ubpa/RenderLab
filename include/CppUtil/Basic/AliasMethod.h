#ifndef _CPPUTIL_BASIC_SAMPLER_ALIAS_METHOD_H_
#define _CPPUTIL_BASIC_SAMPLER_ALIAS_METHOD_H_

#include <vector>

namespace CppUtil {
	namespace Basic {
		class AliasMethod {
		public:
			AliasMethod(const std::vector<float> & distribution) {
				Init(distribution);
			}

		public:
			// 0, 1, ..., n - 1
			int Sample() const;

		private:
			void Init(const std::vector<float> & distribution);

		private:
			struct Item {
				Item() :u(-1), k(-1) {}

				float u; // probability
				int k; // alias
			};

			std::vector<Item> table;
		};
	}
}

#endif // !_CPPUTIL_BASIC_SAMPLER_ALIAS_METHOD_H_
