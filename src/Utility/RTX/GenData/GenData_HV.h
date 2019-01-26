#ifndef _GEN_FS_HV_H_
#define _GEN_FS_HV_H_

#include <Utility/RTX/HitableVisitor.h>

#include <Utility/Basic/Ptr.h>

#include <vector>
#include <map>

namespace RayTracing {
	class Material;
	class GenData_MV;

	// Generate Fragmen Shader -- Hitable Visitor
	class GenData_HV : public HitableVisitor{
		HEAP_OBJ_SETUP(GenData_HV)
	public:
		GenData_HV(std::vector<float> & packData);

		typedef std::map<CppUtility::Other::CPtr<Material>, size_t> MatIdxMap;

		const std::vector<float> & GetSceneData() const { return sceneData; }
		const std::vector<float> & GetPackData() const { return packData; }
		void Accept(CppUtility::Other::Ptr<GenData_MV> genFS_MV);
	private:
		void SetMat(const MatIdxMap & mat2idx);
		using HitableVisitor::Visit;
		virtual void Visit(CppUtility::Other::CPtr<Hitable> hitable);
		virtual void Visit(CppUtility::Other::CPtr<Group> group);
		virtual void Visit(CppUtility::Other::CPtr<Sphere> sphere);
		virtual void Visit(CppUtility::Other::CPtr<BVH_Node> bvhNode);
		virtual void Visit(CppUtility::Other::CPtr<Triangle> triangle);
		virtual void Visit(CppUtility::Other::CPtr<TriMesh> triMesh);
		virtual void Visit(CppUtility::Other::CPtr<Transform> transform);
		virtual void Visit(CppUtility::Other::CPtr<Volume> volume);
		virtual void Visit(CppUtility::Other::CPtr<Sky> sky);
		virtual void Visit(CppUtility::Other::CPtr<Model> model);

		std::vector<float> sceneData;
		std::vector<float> & packData;
		std::map< CppUtility::Other::CPtr<Hitable>, size_t> hitable2idx;
		std::map<CppUtility::Other::CPtr<Material>, std::vector<size_t>> mat2idxVec;
	};
}

#endif // !_GEN_FS_HV_H_
