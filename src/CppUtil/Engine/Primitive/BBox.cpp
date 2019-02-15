#include <CppUtil/Engine/BBox.h>

#include <CppUtil/Basic/Math.h>

#include <vector>
#include <bitset>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

const BBox BBox::Transform(const mat4 & matrix) const {
	vec3 srcP[2] = { minP, maxP };
	vector<vec3> tfmCornerPs;
	for (size_t i = 0; i < 8; i++) {
		bitset<3> binVal(i);
		vec3 cornerP = vec3(srcP[binVal[2]].x, srcP[binVal[1]].y, srcP[binVal[0]].z);
		vec4 tfmCornerPQ = matrix * vec4(cornerP, 1);
		tfmCornerPs.push_back(vec3(tfmCornerPQ) / tfmCornerPQ.w);
	}

	vec3 minP = Math::min(tfmCornerPs);
	vec3 maxP = Math::max(tfmCornerPs);
	return { minP, maxP };
}
