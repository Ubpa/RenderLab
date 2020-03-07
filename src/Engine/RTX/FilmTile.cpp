#include "FilmTile.h"

#include <Engine/Filter/ImgFilter.h>
#include <UGM/val.h>

using namespace Ubpa;


const bboxi2 FilmTile::SampleFrame() const {
	auto minP = (frame[0].cast_to<vecf2>() + vecf2(0.5f) - filter->radius).cast_to<pointi2>();
	auto maxP = (frame[1].cast_to<vecf2>() + filter->radius).cast_to<pointi2>(); // 因为 frame 不包含上边界，所以这里是减去半像素
	return bboxi2(minP, maxP);
}

void FilmTile::AddSample(const pointf2 & pos, const rgbf & radiance) {
	if (radiance.has_nan())
		return;

	const auto minP = pos - filter->radius;
	const auto maxP = pos + filter->radius;

	const int x0 = std::max(static_cast<int>(minP[0] + 0.5f), frame.minP()[0]);
	const int x1 = std::min(static_cast<int>(maxP[0] - 0.5f), frame.maxP()[0]);

	const int y0 = std::max(static_cast<int>(minP[1] + 0.5f), frame.minP()[1]);
	const int y1 = std::min(static_cast<int>(maxP[1] - 0.5f), frame.maxP()[1]);

	const int width = frame.maxP()[0] - frame.minP()[0];
	for (int x = x0; x < x1; x++) {
		int idxX = x - frame.minP()[0];
		for (int y = y0; y < y1; y++) {
			int idxY = y - frame.minP()[1];

			const auto weight = filter->Evaluate(pos - (vecf2(x, y) + vecf2(0.5f)));
			pixels[idxX][idxY].filterWeightSum += weight;
			pixels[idxX][idxY].weightRadianceSum += weight * radiance;
		}
	}
}
