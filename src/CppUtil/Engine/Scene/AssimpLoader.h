#ifndef _CPPUITL_ENGINE_SCENE_ASSIMP_LOADER_H_
#define _CPPUITL_ENGINE_SCENE_ASSIMP_LOADER_H_

#include <3rdParty/assimp/Importer.hpp>
#include <3rdParty/assimp/scene.h>
#include <3rdParty/assimp/postprocess.h>

#include <CppUtil/Basic/Ptr.h>
#include <string>

namespace CppUtil {
	namespace Engine {
		class SObj;
		class TriMesh;

		namespace AssimpLoader {
			const Basic::Ptr<SObj> Load(const std::string & path);
			const Basic::Ptr<SObj> LoadNode(aiNode *node, const aiScene *scene);
			const Basic::Ptr<TriMesh> LoadMesh(aiMesh *mesh, const aiScene *scene);
		};
	}
}

#endif // !_CPPUITL_ENGINE_SCENE_ASSIMP_LOADER_H_
