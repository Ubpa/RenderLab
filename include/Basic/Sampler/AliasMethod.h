#pragma once

#include <vector>

namespace Ubpa {
	class AliasMethod {
	public:
		AliasMethod(const std::vector<double>& distribution = std::vector<double>()) {
			Init(distribution);
		}

	public:
		void Init(const std::vector<double>& distribution);

		void Clear() { table.clear(); }

		// 0, 1, ..., n - 1
		int Sample() const;
		int Sample(double& p) const;

		double P(int i) const;

	private:
		struct Item {
			Item() :u(-1), k(-1) {}

			double p; // orig probability
			double u; // choose probability
			int k; // alias
		};

		std::vector<Item> table;
	};
}
