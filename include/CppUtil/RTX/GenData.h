#ifndef _FS_GENERATOR_H_
#define _FS_GENERATOR_H_

#include <CppUtil/Basic/Ptr.h>

#include <map>
#include <vector>

namespace CppUtil {
	namespace Basic {
		class Image;
	}
}

namespace RTX {
	class Hitable;
	class Group;
	class GenData_HV;
	class GenData_MV;
	class GenData_TV;

	class GenData {
	public:
		typedef std::map<CppUtil::Basic::CPtr<CppUtil::Basic::Image>, size_t> ImgIdxMap;
		typedef std::vector<CppUtil::Basic::CPtr<CppUtil::Basic::Image> > SkyboxImgs;

		GenData(CppUtil::Basic::CPtr<Hitable> scene);
		
		const std::vector<float> GetSceneData() const;
		const std::vector<float> GetMatData() const;
		const std::vector<float> GetTexData() const;
		const std::vector<float> GetPackData() const;
		const ImgIdxMap GetImgIdxMap() const;
		const SkyboxImgs & GetSkyboxImgs() const;
	private:

		CppUtil::Basic::Ptr<GenData_HV> hitableVisitor;
		CppUtil::Basic::Ptr<GenData_MV> matVisitor;
		CppUtil::Basic::Ptr<GenData_TV> texVisitor;

		CppUtil::Basic::CPtr<Group> root;

		std::vector<float> packData;
	};
}

#endif // !_SHADER_GENERATOR_H_
