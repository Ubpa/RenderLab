#pragma once

#include "Film.h"

namespace Ubpa {
	class FilmTile : public HeapObj {
	public:
		FilmTile(const bboxi2& frame, Ptr<ImgFilter> filter)
			: frame(frame),
			filter(filter),
			pixels(frame.diagonal()[0], std::vector<Film::Pixel>(frame.diagonal()[1])) { }

	protected:
		virtual ~FilmTile() = default;

	public:
		// Frame 不包含右上边界
		const bboxi2 SampleFrame() const;

		void AddSample(const pointf2& pos, const rgbf& radiance);

		const bboxi2 GetFrame() const { return frame; }
		const std::vector<vali2> AllPos() const {
			std::vector<vali2> posArr;
			posArr.reserve(frame.area());
			for (int x = frame.minP()[0]; x < frame.maxP()[0]; x++) {
				for (int y = frame.minP()[1]; y < frame.maxP()[1]; y++) {
					posArr.push_back({ x,y });
				}
			}
			return posArr;
		}
		const Film::Pixel& At(const vali2& pos) const {
			assert(pos[0] >= frame.minP()[0] && pos[0] < frame.maxP()[0]);
			assert(pos[1] >= frame.minP()[1] && pos[1] < frame.maxP()[1]);
			return pixels[pos[0] - frame.minP()[0]][pos[1] - frame.minP()[1]];
		}

	public:
		static Ptr<FilmTile> New(const bboxi2& frame, Ptr<ImgFilter> filter) {
			return Ubpa::New<FilmTile>(frame, filter);
		}

	private:
		const bboxi2 frame;
		std::vector<std::vector<Film::Pixel>> pixels;

		Ptr<ImgFilter> filter;
	};
}
