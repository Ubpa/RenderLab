#ifndef _CPPUITL_ENGINE_SCENE_ASSIMP_LOADER_H_
#define _CPPUITL_ENGINE_SCENE_ASSIMP_LOADER_H_

#include <3rdParty/assimp/Importer.hpp>
#include <3rdParty/assimp/scene.h>
#include <3rdParty/assimp/postprocess.h>

#include <Basic/Ptr.h>

#include <string>
#include <map>
#include <array>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class SObj;
		class TriMesh;

		namespace AssimpLoader {
			const Basic::Ptr<SObj> Load(const std::string & path);
			using Str2Img = std::map<std::string, Basic::Ptr<Basic::Image>>;
			const Basic::Ptr<SObj> LoadNode(Str2Img & str2img, const std::string & dir, aiNode *node, const aiScene *scene);
			void LoadMesh(Str2Img & str2img, const std::string & dir, aiMesh *mesh, const aiScene *scene, Basic::Ptr<SObj> sobj);
			Basic::Ptr<Basic::Image> LoadTexture(Str2Img & str2img, const std::string & dir, aiMaterial* material, aiTextureType type);
		};
	}
}

#endif // !_CPPUITL_ENGINE_SCENE_ASSIMP_LOADER_H_
