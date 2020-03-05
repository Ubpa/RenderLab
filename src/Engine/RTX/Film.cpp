#include "Film.h"

#include "FilmTile.h"

#include <Engine/Filter.h>
#include <Basic/Image.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;

Film::Film(Basic::Ptr<Basic::Image> img, Basic::Ptr<Filter> filter)
	: resolution(img->GetWidth(), img->GetHeight()),
	pixels(img->GetWidth(), std::vector<Pixel>(img->GetHeight())),
	frame({ 0,0 }, { img->GetWidth(),img->GetHeight() }),
	filter(filter),
	img(img)
{
	assert(img != nullptr && img->IsValid());
	assert(img->GetChannel() == 3);
}

const Ptr<FilmTile> Film::GenFilmTile(const Ubpa::bboxi2 & frame) const {
	return FilmTile::New(frame, filter);
}

void Film::MergeFilmTile(Basic::Ptr<FilmTile> filmTile) {
	for (const auto pos : filmTile->AllPos()) {
		pixels[pos[0]][pos[1]] += filmTile->At(pos);
		img->SetPixel(pos, pixels[pos[0]][pos[1]].ToRadiance());
	}
}
