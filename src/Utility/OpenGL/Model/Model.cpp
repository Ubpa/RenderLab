#include <Utility/OGL/Model.h>
#include <Utility/OGL/Texture.h>

#include <iostream>

using namespace CppUtility::OpenGL;
using namespace std;

// constructor, expects a filepath to a 3D model.
Model::Model(const string & path, bool gamma)
	: gammaCorrection(gamma)
{
	LoadModel(path);
}

// draws the model, and thus all its meshes
void Model::Draw(const Shader & shader, const string & materialPrefix)
{
	for (size_t i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader, materialPrefix);
}

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel(const string & path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::ProcessNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}

}

Mesh Model::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Mesh::Vertex> vertices;
	vector<size_t> indices;
	vector<Mesh::TextureInfo> textureInfos;

	// Walk through each of the mesh's vertices
	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Mesh::Vertex vertex;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		// tangent
		//vector.x = mesh->mTangents[i].x;
		//vector.y = mesh->mTangents[i].y;
		//vector.z = mesh->mTangents[i].z;
		//vertex.Tangent = vector;
		// bitangent
		//vector.x = mesh->mBitangents[i].x;
		//vector.y = mesh->mBitangents[i].y;
		//vector.z = mesh->mBitangents[i].z;
		//vertex.Bitangent = vector;
		
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Mesh::TextureInfo> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textureInfos.insert(textureInfos.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Mesh::TextureInfo> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textureInfos.insert(textureInfos.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Mesh::TextureInfo> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textureInfos.insert(textureInfos.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Mesh::TextureInfo> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textureInfos.insert(textureInfos.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textureInfos);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
vector<Mesh::TextureInfo> Model::LoadMaterialTextures(aiMaterial *mat, aiTextureType type, const string & typeName)
{
	vector<Mesh::TextureInfo> textureInfos;
	for (size_t i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (size_t j = 0; j < textureInfos_loaded.size(); j++)
		{
			if (std::strcmp(textureInfos_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textureInfos.push_back(textureInfos_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Mesh::TextureInfo textureInfo;
			string path = directory + "/" + str.C_Str();
			Texture tex(path);
			if (tex.IsValid() == false) {
				printf("ERROR:Texture [%s] load fail.\n", path.c_str());
				exit(1);
			}
			textureInfo.id = tex.GetID();
			textureInfo.type = typeName;
			textureInfo.path = str.C_Str();
			textureInfos.push_back(textureInfo);
			textureInfos_loaded.push_back(textureInfo);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textureInfos;
}