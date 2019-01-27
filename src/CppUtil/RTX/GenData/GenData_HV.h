#ifndef _GEN_FS_HV_H_
#define _GEN_FS_HV_H_

#include <CppUtil/RTX/HitableVisitor.h>

#include <CppUtil/Basic/Ptr.h>

#include <vector>
#include <map>

namespace RTX {
	class Material;
	class GenData_MV;

	// Generate Fragmen Shader -- Hitable Visitor
	class GenData_HV : public HitableVisitor{
		HEAP_OBJ_SETUP(GenData_HV)
	public:
		GenData_HV(std::vector<float> & packData);

		typedef std::map<CppUtil::Basic::CPtr<Material>, size_t> MatIdxMap;

		const std::vector<float> & GetSceneData() const { return sceneData; }
		const std::vector<float> & GetPackData() const { return packData; }
		void Accept(CppUtil::Basic::Ptr<GenData_MV> genFS_MV);
	private:
		void SetMat(const MatIdxMap & mat2idx);
		using HitableVisitor::Visit;
		virtual void Visit(CppUtil::Basic::CPtr<Hitable> hitable);
		virtual void Visit(CppUtil::Basic::CPtr<Group> group);
		virtual void Visit(CppUtil::Basic::CPtr<Sphere> sphere);
		virtual void Visit(CppUtil::Basic::CPtr<BVH_Node> bvhNode);
		virtual void Visit(CppUtil::Basic::CPtr<Triangle> triangle);
		virtual void Visit(CppUtil::Basic::CPtr<TriMesh> triMesh);
		virtual void Visit(CppUtil::Basic::CPtr<Transform> transform);
		virtual void Visit(CppUtil::Basic::CPtr<Volume> volume);
		virtual void Visit(CppUtil::Basic::CPtr<Sky> sky);
		virtual void Visit(CppUtil::Basic::CPtr<Model> model);

		std::vector<float> sceneData;
		std::vector<float> & packData;
		std::map< CppUtil::Basic::CPtr<Hitable>, size_t> hitable2idx;
		std::map<CppUtil::Basic::CPtr<Material>, std::vector<size_t>> mat2idxVec;
	};
}

#endif // !_GEN_FS_HV_H_
