#ifndef _TRI_MESH_H_
#define _TRI_MESH_H_

#include <Utility/RTX/BVH_Node.h>

namespace RayTracing {
	class TriMesh : public BVH_Node{
		HITABLE_SETUP(TriMesh)
	public:
		TriMesh(const std::vector<Vertex> & vertexs, Material::CPtr material = NULL);

		inline bool IsValid() const { return isValid; };
	protected:
		bool isValid;
	};
}

#endif // !_TRI_MESH_H_
