#pragma once

#include <Basic/Ptr.h>

#include <string>
#include <map>
#include <array>

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;
enum aiTextureType;

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
