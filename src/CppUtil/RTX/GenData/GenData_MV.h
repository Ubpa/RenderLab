#ifndef _GEN_FS_MV_H_
#define _GEN_FS_MV_H_

#include <CppUtil/RTX/MatVisitor.h>

#include <CppUtil/Basic/Ptr.h>

#include <vector>
#include <map>

namespace RTX {
	class Texture;
	class GenData_TV;

	// Generate Fragmen Shader -- Material Visitor
	class GenData_MV : public MatVisitor{
		HEAP_OBJ_SETUP(GenData_MV)
	public:
		typedef std::map<CppUtil::Basic::CPtr<Material>, size_t> MatIdxMap;
		typedef std::map<CppUtil::Basic::CPtr<Texture>, size_t> TexIdxMap;

		void Accept(CppUtil::Basic::Ptr<GenData_TV> genFS_TV);
		const MatIdxMap & GetMatIdxMap() const { return mat2idx; }
		const std::vector<float> & GetMatData() const { return matData; };
	private:
		void SetTex(const TexIdxMap & tex2idx);

		virtual void Visit(CppUtil::Basic::CPtr<Lambertian> lambertian);
		virtual void Visit(CppUtil::Basic::CPtr<Metal> metal);
		virtual void Visit(CppUtil::Basic::CPtr<Dielectric> dielectric);
		virtual void Visit(CppUtil::Basic::CPtr<Light> light);
		virtual void Visit(CppUtil::Basic::CPtr<Isotropic> isotropic);
		using MatVisitor::Visit;

		std::vector<float> matData;
		std::map<CppUtil::Basic::CPtr<Texture>, std::vector<size_t>> tex2idxVec;
		MatIdxMap mat2idx;
	};
}

#endif // !_GEN_FS_HV_H_
