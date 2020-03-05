#pragma once

#include <3rdParty/assimp/Importer.hpp>
#include <3rdParty/assimp/scene.h>
#include <3rdParty/assimp/postprocess.h>

#include <Basic/Ptr.h>

#include <string>
#include <map>
#include <array>

namespace Ubpa {
	class Image;
	class SObj;
	class TriMesh;

	namespace AssimpLoader {
		const Ptr<SObj> Load(const std::string& path);
		using Str2Img = std::map<std::string, Ptr<Image>>;
		const Ptr<SObj> LoadNode(Str2Img& str2img, const std::string& dir, aiNode* node, const aiScene* scene);
		void LoadMesh(Str2Img& str2img, const std::string& dir, aiMesh* mesh, const aiScene* scene, Ptr<SObj> sobj);
		Ptr<Image> LoadTexture(Str2Img& str2img, const std::string& dir, aiMaterial* material, aiTextureType type);
	};
}
