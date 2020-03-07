#include "Film.h"

#include "FilmTile.h"

#include <Engine/Filter/ImgFilter.h>
#include <Basic/Image.h>

using namespace Ubpa;

Film::Film(Ptr<Image> img, Ptr<ImgFilter> filter)
	: resolution(img->GetWidth(), img->GetHeight()),
	pixels(img->GetWidth(), std::vector<Pixel>(img->GetHeight())),
	frame({ 0,0 }, { img->GetWidth(),img->GetHeight() }),
	filter(filter),
	img(img)
{
	assert(img != nullptr && img->IsValid());
	assert(img->GetChannel() == 3);
}

const Ptr<FilmTile> Film::GenFilmTile(const bboxi2 & frame) const {
	return FilmTile::New(frame, filter);
}

void Film::MergeFilmTile(Ptr<FilmTile> filmTile) {
	for (const auto pos : filmTile->AllPos()) {
		pixels[pos[0]][pos[1]] += filmTile->At(pos);
		img->SetPixel(pos, pixels[pos[0]][pos[1]].ToRadiance());
	}
}
