#ifndef _FS_GENERATOR_H_
#define _FS_GENERATOR_H_

#include <Utility/Basic/Ptr.h>

#include <map>
#include <vector>

namespace CppUtility {
	namespace Other {
		class Image;
	}
}

namespace RayTracing {
	class Hitable;
	class Group;
	class GenData_HV;
	class GenData_MV;
	class GenData_TV;

	class GenData {
	public:
		typedef std::map<CppUtility::Other::CPtr<CppUtility::Other::Image>, size_t> ImgIdxMap;
		typedef std::vector<CppUtility::Other::CPtr<CppUtility::Other::Image> > SkyboxImgs;

		GenData(CppUtility::Other::CPtr<Hitable> scene);
		
		const std::vector<float> GetSceneData() const;
		const std::vector<float> GetMatData() const;
		const std::vector<float> GetTexData() const;
		const std::vector<float> GetPackData() const;
		const ImgIdxMap GetImgIdxMap() const;
		const SkyboxImgs & GetSkyboxImgs() const;
	private:

		CppUtility::Other::Ptr<GenData_HV> hitableVisitor;
		CppUtility::Other::Ptr<GenData_MV> matVisitor;
		CppUtility::Other::Ptr<GenData_TV> texVisitor;

		CppUtility::Other::CPtr<Group> root;

		std::vector<float> packData;
	};
}

#endif // !_SHADER_GENERATOR_H_
