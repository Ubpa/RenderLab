#ifndef _OPTIX_AI_DENOISER_H_
#define _OPTIX_AI_DENOISER_H_

#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class OptixAIDenoiser {
		public:
			static OptixAIDenoiser & GetInstance() {
				static OptixAIDenoiser denoiser;
				return denoiser;
			}

			void Denoise(Basic::Ptr<Basic::Image> img);
		private:
			OptixAIDenoiser() : init(false) { }

		private:
			bool init;
			int width;
			int height;
		};
	}
}

#endif // !_OPTIX_AI_DENOISER_H_
