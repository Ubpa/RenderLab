#ifndef _CPPUTIL_ENGINE_RTX_FILM_TILE_H_
#define _CPPUTIL_ENGINE_RTX_FILM_TILE_H_

#include "Film.h"

namespace CppUtil {
	namespace Engine {
		class FilmTile : public Basic::HeapObj {
		public:
			FilmTile(const Framei & frame, Basic::Ptr<Filter> filter)
				: frame(frame),
				filter(filter),
				pixels(frame.Diagonal().x, std::vector<Film::Pixel>(frame.Diagonal().y)) { }

		protected:
			virtual ~FilmTile() = default;

		public:
			// Frame 不包含右上边界
			const Framei SampleFrame() const;

			void AddSample(const Point2f & pos, const RGBf & radiance);

			const Framei GetFrame() const { return frame; }
			const std::vector<Point2i> AllPos() const {
				std::vector<Point2i> posArr;
				posArr.reserve(frame.Area());
				for (int x = frame.minP.x; x < frame.maxP.x; x++) {
					for (int y = frame.minP.y; y < frame.maxP.y; y++) {
						posArr.push_back({ x,y });
					}
				}
				return posArr;
			}
			const Film::Pixel & At(const Point2i & pos) const {
				assert(pos.x >= frame.minP.x && pos.x < frame.maxP.x);
				assert(pos.y >= frame.minP.y && pos.y < frame.maxP.y);
				return pixels[pos.x - frame.minP.x][pos.y - frame.minP.y];
			}

		public:
			static Basic::Ptr<FilmTile> New(const Framei & frame, Basic::Ptr<Filter> filter) {
				return Basic::New<FilmTile>(frame, filter);
			}

		private:
			const Framei frame;
			std::vector<std::vector<Film::Pixel>> pixels;

			Basic::Ptr<Filter> filter;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_RTX_FILM_TILE_H_
