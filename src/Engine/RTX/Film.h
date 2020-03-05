#pragma once

#include <Basic/HeapObj.h>

#include <UGM/rgb.h>
#include <UGM/point.h>
#include <UGM/val.h>

#include <Basic/Array2D.h>
#include <UGM/bbox.h>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class Filter;
		class FilmTile;

		class Film : public Basic::HeapObj {
		public:
			Film(Basic::Ptr<Basic::Image> img, Basic::Ptr<Filter> filter);

		protected:
			virtual ~Film() = default;

		public:
			static Basic::Ptr<Film> New(Basic::Ptr<Basic::Image> img, Basic::Ptr<Filter> filter) {
				return Basic::New<Film>(img, filter);
			}

		public:
			const Basic::Ptr<FilmTile> GenFilmTile(const Ubpa::bboxi2 & frame) const;
			void MergeFilmTile(Basic::Ptr<FilmTile> filmTile);

		private:
			friend class FilmTile;

			struct Pixel {
				Pixel() : weightRadianceSum(0.f), filterWeightSum(0.f) { }

				Ubpa::rgbf weightRadianceSum;
				float filterWeightSum;

				Pixel & operator+=(const Pixel & pixel) {
					weightRadianceSum += pixel.weightRadianceSum;
					filterWeightSum += pixel.filterWeightSum;
					return *this;
				}

				const Ubpa::rgbf ToRadiance() const {
					if (filterWeightSum == 0)
						return Ubpa::rgbf(0.f);

					return weightRadianceSum / filterWeightSum;
				}
			};

		private:
			Basic::Ptr<Basic::Image> img;
			const Ubpa::vali2 resolution;
			std::vector<std::vector<Pixel>> pixels;

			const Ubpa::bboxi2 frame; // 不包括右上的边界
			Basic::Ptr<Filter> filter;
		};
	}
}
