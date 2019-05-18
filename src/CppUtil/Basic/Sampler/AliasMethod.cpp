#include <CppUtil/Basic/AliasMethod.h>

#include <CppUtil/Basic/Math.h>

using namespace CppUtil::Basic;
using namespace std;

void AliasMethod::Init(const vector<float> & distribution) {
	table.clear();
	const auto num = distribution.size();
	table.resize(num);

	vector<int> overfulls;
	vector<int> underfulls;
	overfulls.reserve(num);
	underfulls.reserve(num);

	for (size_t i = 0; i < num; i++) {
		table[i].u = num * distribution[i];
		if (table[i].u < 1)
			underfulls.push_back(i);
		else if (table[i].u > 1)
			overfulls.push_back(i);
		else
			table[i].k = i;
	}

	while (!overfulls.empty() && !underfulls.empty()) {
		int i = overfulls.back();
		overfulls.pop_back();
		
		int j = underfulls.back();
		underfulls.pop_back();

		table[i].u -= 1 - table[j].u;
		table[j].k = i;

		if (table[i].u > 1)
			underfulls.push_back(i);
	}
	
	for (auto i : overfulls) {
		table[i].u = 1.f;
		table[i].k = i;
	}

	for (auto j : underfulls) {
		table[j].u = 1.f;
		table[j].k = j;
	}
}

int AliasMethod::Sample() const {
	float x = Math::Rand_F();
	auto n = static_cast<int>(table.size());

	// 0, 1, ..., n - 1
	auto i = static_cast<int>(floor(n*x));

	// [0, 1)
	auto y = n * x - i;

	if (y < table[i].u)
		return i;
	else
		return table[i].k;
}
