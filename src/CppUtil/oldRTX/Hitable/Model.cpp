#include <CppUtil/RTX/Model.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include <map>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

Model::Model(const string & path, CppUtil::Basic::CPtr<Material> material)
	: TriMesh(Load(path), material) { }

vector<Vertex> Model::Load(const std::string & path) {
	vector<Vertex> vertices;

	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		return vertices;

	// retrieve the directory path of the filepath
	//directory = path.substr(0, path.find_last_of('/'));
	// process ASSIMP's root node recursively
	Load(scene->mRootNode, scene, vertices);

	return vertices;
}

void Model::Load(const aiNode *node, const aiScene *scene, vector<Vertex> & vertices) {
	// process each mesh located at the current node
	for (size_t i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Load(mesh, scene, vertices);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		Load(node->mChildren[i], scene, vertices);
	}
}

void Model::Load(const aiMesh *mesh, const aiScene *scene, vector<Vertex> & vertices) {
	// compute normal
	map<size_t, vector<uvec3>> idx2fV;
	map<size_t, vec3> idx2n;
	if (mesh->mNormals == NULL) {
		// collect faces
		for (size_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			uvec3 uvec3_face(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
			idx2fV[uvec3_face[0]].push_back(uvec3_face);
			idx2fV[uvec3_face[1]].push_back(uvec3_face);
			idx2fV[uvec3_face[2]].push_back(uvec3_face);
		}
		// compute average normal
		for (auto const pair : idx2fV) {
			vec3 normal(0);
			for (auto const face : pair.second) {
				vec3 p[3];
				for (size_t i = 0; i < 3; i++) {
					p[i].x = mesh->mVertices[face[i]].x;
					p[i].y = mesh->mVertices[face[i]].y;
					p[i].z = mesh->mVertices[face[i]].z;
				}
				normal += normalize(cross(p[1] - p[0], p[2] - p[0]));
			}
			idx2n[pair.first] = normal / (float)pair.second.size();
		}
	}

	// walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (size_t j = 0; j < face.mNumIndices; j++)// mNumIndices == 3
		{
			size_t idx = face.mIndices[j];
			//indices.push_back(face.mIndices[j]);
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

			// positions
			vector.x = mesh->mVertices[idx].x;
			vector.y = mesh->mVertices[idx].y;
			vector.z = mesh->mVertices[idx].z;
			vertex.pos = vector;

			// normals
			if (mesh->mNormals != NULL) {
				vector.x = mesh->mNormals[idx].x;
				vector.y = mesh->mNormals[idx].y;
				vector.z = mesh->mNormals[idx].z;
			}
			else
				vector = idx2n[idx];

			vertex.normal = vector;
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][idx].x;
				vec.y = mesh->mTextureCoords[0][idx].y;
				vertex.u = vec.x;
				vertex.v = vec.y;
			}
			else {
				vertex.u = 0;
				vertex.u = 0;
			}
			// tangent
			//vector.x = mesh->mTangents[idx].x;
			//vector.y = mesh->mTangents[idx].y;
			//vector.z = mesh->mTangents[idx].z;
			//vertex.Tangent = vector;
			// bitangent
			//vector.x = mesh->mBitangents[idx].x;
			//vector.y = mesh->mBitangents[idx].y;
			//vector.z = mesh->mBitangents[idx].z;
			//vertex.Bitangent = vector;

			vertices.push_back(vertex);
		}
	}
	/*
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
	*/
}
