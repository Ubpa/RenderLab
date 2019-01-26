#ifndef _SKYBOX_H_
#define _SKY_BOX_H_

#include <Utility/RTX/Texture.h>

#include <Utility/Basic/Ptr.h>

#include <vector>
#include <string>

namespace CppUtility {
	namespace Other {
		class Image;
	}
}

namespace RayTracing {
	class Skybox : public Texture {
		TEXTURE_SETUP(Skybox)
	public:
		typedef std::vector<CppUtility::Other::CPtr<CppUtility::Other::Image> > SkyboxImgs;
		//right, left, top, bottom, front, back
		Skybox(const std::vector<std::string> & skybox);
		const SkyboxImgs & GetSkyboxImgs() const { return imgs; }
		bool IsValid() const;
	private:
		virtual glm::rgb Value(float u = 0, float v = 0, const glm::vec3 & p = glm::vec3(0)) const;
		SkyboxImgs imgs;
	};
}

#endif // !_SKYBOX_H_
