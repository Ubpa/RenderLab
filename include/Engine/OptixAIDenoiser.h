#pragma once

#include <Basic/Ptr.h>

namespace Ubpa {
	class Image;

	class OptixAIDenoiser {
	public:
		static OptixAIDenoiser& GetInstance() {
			static OptixAIDenoiser denoiser;
			return denoiser;
		}

		void Denoise(Ptr<Image> img);
	};
}
