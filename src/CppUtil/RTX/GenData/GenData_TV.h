#ifndef _GEN_FS_TV_H_
#define _GEN_FS_TV_H_

#include <CppUtil/RTX/TexVisitor.h>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Basic {
		class Image;
	}
}

namespace RTX {
	// Generate Fragmen Shader -- Texture Visitor
	class GenData_TV : public TexVisitor{
		HEAP_OBJ_SETUP(GenData_TV)
	public:
		typedef std::vector<CppUtil::Basic::CPtr<CppUtil::Basic::Image> > SkyboxImgs;

		GenData_TV(std::vector<float> & packData);

		typedef std::map<CppUtil::Basic::CPtr<Texture>, size_t> TexIdxMap;
		typedef std::map<CppUtil::Basic::CPtr<CppUtil::Basic::Image>, size_t> ImgIdxMap;

		const TexIdxMap & GetTexIdxMap() const { return tex2idx; }
		const ImgIdxMap & GetImgIdxMap() const { return img2idx; }
		const SkyboxImgs & GetSkyboxImgs() const { return skyboxImgs; }
		const std::vector<float> & GetTexData() const { return texData; };
	private:
		virtual void Visit(CppUtil::Basic::CPtr<ConstTexture> constTexture);
		virtual void Visit(CppUtil::Basic::CPtr<ImgTexture> imgTexture);
		virtual void Visit(CppUtil::Basic::CPtr<Skybox> skybox);
		using TexVisitor::Visit;

		std::vector<float> texData;
		std::vector<float> & packData;
		TexIdxMap tex2idx;
		ImgIdxMap img2idx;
		SkyboxImgs skyboxImgs;
	};
}

#endif // !_GEN_FS_TV_H_
