#pragma once

#include <Basic/HeapObj.h>

#include <UGM/rgb.h>
#include <UGM/point.h>
#include <UGM/val.h>

#include <Basic/Array2D.h>
#include <UGM/bbox.h>
#include <vector>

namespace Ubpa {
	class Image;
	class ImgFilter;
	class FilmTile;

	class Film : public HeapObj {
	public:
		Film(Ptr<Image> img, Ptr<ImgFilter> filter);

	protected:
		virtual ~Film() = default;

	public:
		static Ptr<Film> New(Ptr<Image> img, Ptr<ImgFilter> filter) {
			return Ubpa::New<Film>(img, filter);
		}

	public:
		const Ptr<FilmTile> GenFilmTile(const bboxi2& frame) const;
		void MergeFilmTile(Ptr<FilmTile> filmTile);

	private:
		friend class FilmTile;

		struct Pixel {
			Pixel() : weightRadianceSum(0.f), filterWeightSum(0.f) { }

			rgbf weightRadianceSum;
			float filterWeightSum;

			Pixel& operator+=(const Pixel& pixel) {
				weightRadianceSum += pixel.weightRadianceSum;
				filterWeightSum += pixel.filterWeightSum;
				return *this;
			}

			const rgbf ToRadiance() const {
				if (filterWeightSum == 0)
					return rgbf(0.f);

				return weightRadianceSum / filterWeightSum;
			}
		};

	private:
		Ptr<Image> img;
		const vali2 resolution;
		std::vector<std::vector<Pixel>> pixels;

		const bboxi2 frame; // 不包括右上的边界
		Ptr<ImgFilter> filter;
	};
}
