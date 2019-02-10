#ifndef _RTX_HITABLE_MODEL_H_
#define _RTX_HITABLE_MODEL_H_

#include <CppUtil/RTX/TriMesh.h>

struct aiNode;
struct aiMesh;
struct aiScene;

namespace RTX{
	class Model : public TriMesh {
		HITABLE_SETUP(Model)
	public:
		Model(const std::string & path, CppUtil::Basic::CPtr<Material> material = NULL);

	private:
		static std::vector<Vertex> Load(const std::string & path);
		static void Load(const aiNode * node, const aiScene * scene, std::vector<Vertex> & vertices);
		static void Load(const aiMesh * mesh, const aiScene * scene, std::vector<Vertex> & vertices);
	};
}

#endif // !_RTX_HITABLE_MODEL_H_
