#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <OpenGL/Mesh.h>

namespace Ubpa {
	class Model
	{
	public:
		/*  Functions   */
		// constructor, expects a filepath to a 3D model.
		Model(const std::string& path, bool gamma = false);

		// draws the model, and thus all its meshes
		void Draw(const Shader& shader, const std::string& materialPrefix = "");

	private:
		/*  Model Data */
		std::vector<Mesh::TextureInfo> textureInfos_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		std::vector<Mesh> meshes;
		std::string directory;
		bool gammaCorrection;

		/*  Functions   */
		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void LoadModel(const std::string& path);

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void ProcessNode(aiNode* node, const aiScene* scene);

		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		std::vector<Mesh::TextureInfo> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
	};
}
