#include "FilmTile.h"

#include <CppUtil/Engine/Filter.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

const Framei FilmTile::SampleFrame() const {
	const Point2i minP = Vec2f(frame[0]) + Vec2f(0.5f) - filter->radius;
	const Point2i maxP = Vec2f(frame[1]) - Vec2f(0.5f) + filter->radius; // 因为 frame 不包含上边界，所以这里是减去半像素
	return Framei(minP, maxP);
}

void FilmTile::AddSample(const Point2f & pos, const RGBf & radiance) {
	const auto minP = pos - filter->radius;
	const auto maxP = pos + filter->radius;

	const int x0 = std::max(static_cast<int>(minP.x + 0.5f), frame.minP.x);
	const int x1 = std::min(static_cast<int>(maxP.x - 0.5f), frame.maxP.x);

	const int y0 = std::max(static_cast<int>(minP.y + 0.5f), frame.minP.y);
	const int y1 = std::min(static_cast<int>(maxP.y - 0.5f), frame.maxP.y);

	const int width = frame.maxP.x - frame.minP.x;
	for (int x = x0; x < x1; x++) {
		int idxX = x - frame.minP.x;
		for (int y = y0; y < y1; y++) {
			int idxY = y - frame.minP.y;

			const auto weight = filter->Evaluate(pos - Point2f(x, y));
			pixels[idxX][idxY].filterWeightSum += weight;
			pixels[idxX][idxY].weightRadianceSum += weight * radiance;
		}
	}
}
