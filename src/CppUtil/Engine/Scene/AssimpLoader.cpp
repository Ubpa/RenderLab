#include "AssimpLoader.h"

#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptMaterial.h>
#include <CppUtil/Engine/CmptTransform.h>
#include <CppUtil/Engine/CmptGeometry.h>

#include <CppUtil/Engine/TriMesh.h>
#include <CppUtil/Engine/BSDF_Diffuse.h>

#include <iostream>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

const Ptr<SObj> AssimpLoader::Load(const std::string & path) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return nullptr;
	}

	// process ASSIMP's root node recursively
	return LoadNode(scene->mRootNode, scene);
}

const Ptr<SObj> AssimpLoader::LoadNode(aiNode *node, const aiScene *scene) {
	auto sobj = SObj::New(nullptr, node->mName.C_Str());

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		auto triMesh = LoadMesh(mesh, scene);
		auto meshObj = SObj::New(sobj, "mesh_" + to_string(i));
		CmptGeometry::New(meshObj, triMesh);
		CmptMaterial::New(meshObj, BSDF_Diffuse::New());
		CmptTransform::New(meshObj);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (uint i = 0; i < node->mNumChildren; i++) {
		auto child = LoadNode(node->mChildren[i], scene);
		sobj->AddChild(child);
	}

	return sobj;
}

const Ptr<TriMesh> AssimpLoader::LoadMesh(aiMesh *mesh, const aiScene *scene) {
	// data to fill
	vector<Point3> poses;
	vector<Point2> texcoords;
	vector<Normalf> normals;
	vector<uint> indices;
	//vector<Mesh::TextureInfo> textureInfos;

	// Walk through each of the mesh's vertices
	for (uint i = 0; i < mesh->mNumVertices; i++)
	{
		// positions
		Point3 pos;
		pos.x = mesh->mVertices[i].x;
		pos.y = mesh->mVertices[i].y;
		pos.z = mesh->mVertices[i].z;
		poses.push_back(pos);

		// normals
		Normalf normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		normals.push_back(normal);

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			Point2 texcoord;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			texcoord.x = mesh->mTextureCoords[0][i].x;
			texcoord.y = mesh->mTextureCoords[0][i].y;
			texcoords.push_back(texcoord);
		}
		else
			texcoords.push_back({ 0,0 });
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
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (uint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (uint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	//vector<Mesh::TextureInfo> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//textureInfos.insert(textureInfos.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	//vector<Mesh::TextureInfo> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//textureInfos.insert(textureInfos.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	//std::vector<Mesh::TextureInfo> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	//textureInfos.insert(textureInfos.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	//std::vector<Mesh::TextureInfo> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	//textureInfos.insert(textureInfos.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	//return Mesh(vertices, indices, textureInfos);

	return TriMesh::New(indices, poses, normals, texcoords);
}
