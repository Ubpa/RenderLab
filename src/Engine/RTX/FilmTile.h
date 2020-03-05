#ifndef _CPPUTIL_ENGINE_RTX_FILM_TILE_H_
#define _CPPUTIL_ENGINE_RTX_FILM_TILE_H_

#include "Film.h"

namespace CppUtil {
	namespace Engine {
		class FilmTile : public Basic::HeapObj {
		public:
			FilmTile(const Ubpa::bboxi2 & frame, Basic::Ptr<Filter> filter)
				: frame(frame),
				filter(filter),
				pixels(frame.diagonal()[0], std::vector<Film::Pixel>(frame.diagonal()[1])) { }

		protected:
			virtual ~FilmTile() = default;

		public:
			// Frame 不包含右上边界
			const Ubpa::bboxi2 SampleFrame() const;

			void AddSample(const Ubpa::pointf2 & pos, const Ubpa::rgbf & radiance);

			const Ubpa::bboxi2 GetFrame() const { return frame; }
			const std::vector<Ubpa::vali2> AllPos() const {
				std::vector<Ubpa::vali2> posArr;
				posArr.reserve(frame.area());
				for (int x = frame.minP()[0]; x < frame.maxP()[0]; x++) {
					for (int y = frame.minP()[1]; y < frame.maxP()[1]; y++) {
						posArr.push_back({ x,y });
					}
				}
				return posArr;
			}
			const Film::Pixel & At(const Ubpa::vali2 & pos) const {
				assert(pos[0] >= frame.minP()[0] && pos[0] < frame.maxP()[0]);
				assert(pos[1] >= frame.minP()[1] && pos[1] < frame.maxP()[1]);
				return pixels[pos[0] - frame.minP()[0]][pos[1] - frame.minP()[1]];
			}

		public:
			static Basic::Ptr<FilmTile> New(const Ubpa::bboxi2 & frame, Basic::Ptr<Filter> filter) {
				return Basic::New<FilmTile>(frame, filter);
			}

		private:
			const Ubpa::bboxi2 frame;
			std::vector<std::vector<Film::Pixel>> pixels;

			Basic::Ptr<Filter> filter;
		};
	}
}

#endif // !_CPPUTIL_ENGINE_RTX_FILM_TILE_H_
