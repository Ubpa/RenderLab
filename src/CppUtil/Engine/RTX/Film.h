#ifndef _CPPUTIL_ENGINE_RTX_FILM_H_
#define _CPPUTIL_ENGINE_RTX_FILM_H_

#include <CppUtil/Basic/HeapObj.h>

#include <CppUtil/Basic/UGM/RGB.h>
#include <CppUtil/Basic/UGM/Point2.h>

#include <CppUtil/Basic/Array2D.h>
#include <CppUtil/Basic/UGM/Frame.h>
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
			const Basic::Ptr<FilmTile> GenFilmTile(const Framei & frame) const;
			void MergeFilmTile(Basic::Ptr<FilmTile> filmTile);

		private:
			friend class FilmTile;

			struct Pixel {
				Pixel() : weightRadianceSum(0.f), filterWeightSum(0.f) { }

				RGBf weightRadianceSum;
				float filterWeightSum;

				Pixel & operator+=(const Pixel & pixel) {
					weightRadianceSum += pixel.weightRadianceSum;
					filterWeightSum += pixel.filterWeightSum;
					return *this;
				}

				const RGBf ToRadiance() const {
					if (filterWeightSum == 0)
						return RGBf(0.f);

					return weightRadianceSum / filterWeightSum;
				}
			};

		private:
			Basic::Ptr<Basic::Image> img;
			const Point2i resolution;
			std::vector<std::vector<Pixel>> pixels;

			const Framei frame; // 不包括右上的边界
			Basic::Ptr<Filter> filter;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_RTX_FILM_H_
