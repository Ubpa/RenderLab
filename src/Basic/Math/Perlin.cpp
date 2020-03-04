#include <Basic/Perlin.h>

#include <Basic/Math.h>

using namespace CppUtil;
using namespace CppUtil::Basic::Math;
using namespace std;

float Perlin::Noise(const Vec3 & p){
	// pi 为整数部分
	const auto pi = static_cast<Vec3i>(p);
	// pf 为小数部分
	const auto pf = p - pi;
	// 8个位置的向量
	// 单位立方体8个顶点, 每个顶点一个随机向量
	Vec3 c[2][2][2];
	for (int dx = 0; dx < 2; dx++) {
		for (int dy = 0; dy < 2; dy++) {
			for (int dz = 0; dz < 2; dz++) {
				size_t idx = permuteX[(pi.x + dx) & 255]
					^ permuteY[(pi.y + dy) & 255]
					^ permuteZ[(pi.z + dz) & 255];
				c[dx][dy][dz] = randVec[idx];
			}
		}
	}
	return PerlinInterp(c, pf.x, pf.y, pf.z);
}

float Perlin::PerlinInterp(const Vec3 c[2][2][2], float u, float v, float w) {
	float uu = u * u*(3 - 2 * u);
	float vv = v * v*(3 - 2 * v);
	float ww = w * w*(3 - 2 * w);
	float sum = 0;
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			for (size_t k = 0; k < 2; k++) {
				// 顶点 到 插值点 的向量
				Vec3 weightVec(u - i, v - j, w - k);
				// 权值 为 插值点 到 [顶点对角] 的 xyz轴向 的 距离(非负) 的乘积
				//Vec3 absWeightVec = abs(Vec3(u-(1-i),v-(1-j),w-(1-k)));
				//float weight = absWeightVec.x * absWeightVec.y * absWeightVec.z;
				float weight = (i*uu + (1 - i)*(1 - uu))
					* (j*vv + (1 - j)*(1 - vv))
					* (k*ww + (1 - k)*(1 - ww));
				// 加权求和, 项为点乘, 从而达到了最值转移的作用(不在整数点上)
				sum += weight * c[i][j][k].Dot(weightVec);
			}
		}
	}
	return sum;
}

float Perlin::Turb(const Vec3 & p, size_t depth){
	float sum = 0;
	Vec3 curP = p;
	float weight = 1.0;
	for (size_t i = 0; i < depth; i++) {
		sum += weight * Noise(curP);
		// weight = pow(0.5, i);
		weight *= 0.5;
		// curP = p * pow(2, i);
		curP *= 2;
	}
	//float abs --> fabs
	return fabs(sum);
}

vector<size_t> Perlin::GenPermute(size_t n) {
	vector<size_t> rst(n);
	for (size_t i = 0; i < n; i++)
		rst[i] = i;

	Math::Permute(rst);
	return rst;
}

vector<Vec3> Perlin::GenRandVec(size_t n) {
	vector<Vec3> rst(n);
	for (size_t i = 0; i < n; ++i) {
		// sample in sphere
		float Xi1 = Rand_F();
		float Xi2 = Rand_F();
		float Xi3 = Rand_F();

		float cosTheta = 1 - 2.f * Xi1;
		float sinTheta = sqrt(1 - cosTheta * cosTheta);
		float phi = 2 * PI * Xi2;
		float r = pow(Xi3, 1.f / 3.f);

		rst[i].x = r * sinTheta * cos(phi);
		rst[i].y = r * sinTheta * sin(phi);
		rst[i].z = r * cosTheta;
	}

	return rst;
}


vector<Vec3> Perlin::randVec = Perlin::GenRandVec(256);
vector<size_t> Perlin::permuteX = Perlin::GenPermute(256);
vector<size_t> Perlin::permuteY = Perlin::GenPermute(256);
vector<size_t> Perlin::permuteZ = Perlin::GenPermute(256);