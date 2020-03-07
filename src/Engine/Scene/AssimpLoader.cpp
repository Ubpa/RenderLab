#include "AssimpLoader.h"

#if defined(USE_ASSIMP) && USE_ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#endif

#include <Engine/Scene/SObj.h>

#include <Engine/Scene/CmptMaterial.h>
#include <Engine/Scene/CmptTransform.h>
#include <Engine/Scene/CmptGeometry.h>

#include <Engine/Primitive/TriMesh.h>
#include <Engine/Material/BSDF_Frostbite.h>

#include <Basic/StrAPI.h>
#include <Basic/Image.h>

#include <limits>

#include <iostream>

using namespace Ubpa;

using namespace std;

const Ptr<SObj> AssimpLoader::Load(const std::string & path) {
#if defined(USE_ASSIMP) && USE_ASSIMP
	bool isSupport = false;
	const string support[2]= { ".obj", ".fbx"};
	auto lowerCasePath = StrAPI::LowerCase(path);
	for (auto postfix : support) {
		if (StrAPI::IsEndWith(lowerCasePath, postfix)) {
			isSupport = true;
			break;
		}
	}
	if (!isSupport)
		return nullptr;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenNormals);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return nullptr;
	}

	auto dir = StrAPI::DelTailAfter(path, '/');

	// process ASSIMP's root node recursively
	Str2Img str2img;
	return LoadNode(str2img, dir, scene->mRootNode, scene);
#else
	cout << "ERROR::ASSIMP:"<< endl <<
		"\t" << "no assimp because not found it when cmake, read setup.md for more details" << endl;
	return nullptr;
#endif
}

const Ptr<SObj> AssimpLoader::LoadNode(Str2Img & str2img, const string & dir, aiNode *node, const aiScene *scene) {
#if defined(USE_ASSIMP) && USE_ASSIMP
	auto sobj = SObj::New(nullptr, node->mName.C_Str());
	CmptTransform::New(sobj);

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		auto meshObj = SObj::New(sobj, "mesh_" + to_string(i));
		CmptTransform::New(meshObj);
		LoadMesh(str2img, dir, mesh, scene, meshObj);
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned i = 0; i < node->mNumChildren; i++) {
		auto child = LoadNode(str2img, dir, node->mChildren[i], scene);
		sobj->AddChild(child);
	}

	return sobj;
#else
	cout << "ERROR::ASSIMP:" << endl <<
		"\t" << "no assimp because not found it when cmake, read setup.md for more details" << endl;
	return nullptr;
#endif
}

void AssimpLoader::LoadMesh(Str2Img & str2img, const string & dir, aiMesh *mesh, const aiScene *scene, Ptr<SObj> sobj) {
#if defined(USE_ASSIMP) && USE_ASSIMP
	// data to fill
	vector<pointf3> poses;
	vector<pointf2> texcoords;
	vector<normalf> normals;
	vector<unsigned> indices;
	vector<normalf> tangents;
	//vector<Mesh::TextureInfo> textureInfos;

	// Walk through each of the mesh's vertices
	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		// positions
		pointf3 pos;
		pos[0] = mesh->mVertices[i][0];
		pos[1] = mesh->mVertices[i][1];
		pos[2] = mesh->mVertices[i][2];
		poses.push_back(pos);

		// normals
		if (mesh->mNormals) {
			normalf normal;
			normal[0] = mesh->mNormals[i][0];
			normal[1] = mesh->mNormals[i][1];
			normal[2] = mesh->mNormals[i][2];
			normals.push_back(normal);
		}

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			pointf2 texcoord;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			texcoord[0] = mesh->mTextureCoords[0][i][0];
			texcoord[1] = mesh->mTextureCoords[0][i][1];
			texcoords.push_back(texcoord);
		}
		else
			texcoords.push_back({ 0,0 });
		// tangent
		if (mesh->mTangents)
			tangents.push_back({ mesh->mTangents[i][0], mesh->mTangents[i][1], mesh->mTangents[i][2] });
		// bitangent
		//vector[0] = mesh->mBitangents[i][0];
		//vector[1] = mesh->mBitangents[i][1];
		//vector[2] = mesh->mBitangents[i][2];
		//vertex.Bitangent = vector;
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// offset, scale
	pointf3 minP(std::numeric_limits<float>::max());
	pointf3 maxP(-std::numeric_limits<float>::max());
	for (const auto & pos : poses) {
		minP = pointf3::min(minP, pos);
		maxP = pointf3::max(maxP, pos);
	}
	auto offset = -pointf3::mid(minP,maxP).cast_to<vecf3>();
	float scale = sqrt(3.f / (maxP - minP).norm2());
	for (auto & pos : poses)
		pos = (scale * (pos.cast_to<vecf3>() + offset)).cast_to<pointf3>();

	auto triMesh = TriMesh::New(indices, poses, normals, texcoords, tangents);
	CmptGeometry::New(sobj, triMesh);

	auto bsdf = BSDF_Frostbite::New();
	CmptMaterial::New(sobj, bsdf);

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	/*for (int i = 0; i <= 12; i++) {
		auto n = material->GetTextureCount(static_cast<aiTextureType>(i));
		printf("%d : %d\n", i, n);
	}*/

	bsdf->albedoTexture = LoadTexture(str2img, dir, material, aiTextureType_DIFFUSE);
	bsdf->metallicTexture = LoadTexture(str2img, dir, material, aiTextureType_SPECULAR);
	auto shininess = LoadTexture(str2img, dir, material, aiTextureType_SHININESS);
	if(shininess)
		bsdf->roughnessTexture = shininess->inverse();
	bsdf->normalTexture = LoadTexture(str2img, dir, material, aiTextureType_NORMALS);
	bsdf->aoTexture = LoadTexture(str2img, dir, material, aiTextureType_AMBIENT);
#else
	cout << "ERROR::ASSIMP:" << endl <<
		"\t" << "no assimp because not found it when cmake, read setup.md for more details" << endl;
#endif
}

Ptr<Image> AssimpLoader::LoadTexture(Str2Img & str2img, const string & dir, aiMaterial* material, aiTextureType type) {
#if defined(USE_ASSIMP) && USE_ASSIMP
	auto num = material->GetTextureCount(type);
	if (num == 0)
		return nullptr;

	if (num >= 2) {
		printf("WARNNING::AssimpLoader::LoadMaterial:\n"
			"\t""texture(type %d) >= 2\n", static_cast<int>(type));
	}

	aiString str;
	material->GetTexture(type, 0, &str);

	string path = dir + "/" + str.C_Str();

	if (str2img.find(path) != str2img.end())
		return str2img[path];

	auto img = Image::New(path);
	if (!img->IsValid()) {
		printf("ERROR::AssimpLoader::LoadMeshTexture:"
			"\t""[%s] load fail.\n", path.c_str());
		return nullptr;
	}
	str2img[path] = img;
	return img;
#else
	cout << "ERROR::ASSIMP:" << endl <<
		"\t" << "no assimp because not found it when cmake, read setup.md for more details" << endl;
	return nullptr;
#endif
}
