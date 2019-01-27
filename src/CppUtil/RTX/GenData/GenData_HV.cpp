#include "GenData_HV.h"
#include "GenData_MV.h"

#include <CppUtil/RTX/Model.h>
#include <CppUtil/RTX/Sky.h>
#include <CppUtil/RTX/Volume.h>
#include <CppUtil/RTX/Group.h>
#include <CppUtil/RTX/Sphere.h>
#include <CppUtil/RTX/BVH_Node.h>
#include <CppUtil/RTX/Triangle.h>
#include <CppUtil/RTX/TriMesh.h>
#include <CppUtil/RTX/Transform.h>

using namespace RTX;
using namespace glm;
using namespace std;

//HT : Hitable Type
const float HT_Sphere    = 0.0f;
const float HT_Group     = 1.0f;
const float HT_BVH_Node  = 2.0f;
const float HT_Triangle  = 3.0f;
const float HT_TriMesh   = 4.0f;
const float HT_Transform = 5.0f;
const float HT_Volume    = 6.0f;
const float HT_Sky       = 7.0f;

GenData_HV::GenData_HV(vector<float> & packData)
	: packData(packData) { }

void GenData_HV::Visit(Hitable::CPtr hitable) {
	if (hitable == NULL)
		return;

	if(hitable->GetMat())
		mat2idxVec[hitable->GetMat()].push_back(sceneData.size());
	sceneData.push_back(-1);

	sceneData.push_back(hitable->IsMatCoverable());
}

void GenData_HV::Visit(Sphere::CPtr sphere) {
	if (sphere == NULL)
		return;

	auto targetPair = hitable2idx.find(sphere);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[sphere] = sceneData.size();

	sceneData.push_back(HT_Sphere);

	Visit(static_cast<const Hitable::CPtr>(sphere));

	sceneData.push_back(packData.size() / 4);

	vec3 center = sphere->GetCenter();
	packData.push_back(center.x);
	packData.push_back(center.y);
	packData.push_back(center.z);
	packData.push_back(sphere->GetRadius());
}

void GenData_HV::Visit(Group::CPtr group) {
	if (group == NULL)
		return;

	auto targetPair = hitable2idx.find(group);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[group] = sceneData.size();

	sceneData.push_back(HT_Group);

	Visit(static_cast<const Hitable::CPtr>(group));

	size_t childrenSize = group->GetChildren().size();
	//sceneData.push_back(childrenSize);

	size_t curChildIt = sceneData.size();
	for (size_t i = 0; i < childrenSize; i++)
		sceneData.push_back(-float(hitable2idx[group]));
	
	// childrenEnd 用以标识结尾
	sceneData.push_back(-float(hitable2idx[group]));

	for (auto const & child : group->GetChildren()) {
		if (child == NULL)
			continue;

		auto targetHitableIdx = hitable2idx.find(child);
		if (targetHitableIdx != hitable2idx.end()) {
			sceneData[curChildIt++] = targetHitableIdx->second;
			continue;
		}

		sceneData[curChildIt++] = sceneData.size();
		child->Accept(This());
	}
}

void GenData_HV::Visit(BVH_Node::CPtr bvhNode) {
	if (bvhNode == NULL)
		return;

	auto targetPair = hitable2idx.find(bvhNode);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[bvhNode] = sceneData.size();

	sceneData.push_back(HT_BVH_Node);
	Visit(static_cast<const Hitable::CPtr>(bvhNode));

	sceneData.push_back(packData.size() / 4);

	const AABB box = bvhNode->GetBoundingBox();
	packData.push_back(box.GetMinP().x);
	packData.push_back(box.GetMinP().y);
	packData.push_back(box.GetMinP().z);
	packData.push_back(0);
	packData.push_back(box.GetMaxP().x);
	packData.push_back(box.GetMaxP().y);
	packData.push_back(box.GetMaxP().z);
	packData.push_back(0);


	size_t curChildIt = sceneData.size();
	// left
	if(bvhNode->GetLeft())
		sceneData.push_back(-1);
	// right
	if(bvhNode->GetRight())
		sceneData.push_back(-1);
	
	// childrenEnd 用以标识结尾
	sceneData.push_back(-float(hitable2idx[bvhNode]));

	if (bvhNode->GetLeft()) {
		auto targetLeftIdx = hitable2idx.find(bvhNode->GetLeft());
		if (targetLeftIdx == hitable2idx.end()) {
			sceneData[curChildIt++] = sceneData.size();
			bvhNode->GetLeft()->Accept(This());
		}else
			sceneData[curChildIt++] = targetLeftIdx->second;
	}

	if (bvhNode->GetRight()) {
		auto targetRightIdx = hitable2idx.find(bvhNode->GetRight());
		if (targetRightIdx == hitable2idx.end()) {
			sceneData[curChildIt++] = sceneData.size();
			bvhNode->GetRight()->Accept(This());
		}
		else
			sceneData[curChildIt++] = targetRightIdx->second;
	}
}

void GenData_HV::Visit(Triangle::CPtr triangle) {
	if (triangle == NULL)
		return;

	auto targetPair = hitable2idx.find(triangle);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[triangle] = sceneData.size();

	sceneData.push_back(HT_Triangle);

	Visit(static_cast<Hitable::CPtr>(triangle));

	const Vertex ABC[3] = { triangle->GetA(),triangle->GetB(),triangle->GetC() };
	sceneData.push_back(packData.size() / 4);
	for (size_t i = 0; i < 3; i++) {
		packData.push_back(ABC[i].pos.x);
		packData.push_back(ABC[i].pos.y);
		packData.push_back(ABC[i].pos.z);
		packData.push_back(ABC[i].u);
		packData.push_back(ABC[i].normal.x);
		packData.push_back(ABC[i].normal.y);
		packData.push_back(ABC[i].normal.z);
		packData.push_back(ABC[i].v);
	}
}

void GenData_HV::Visit(TriMesh::CPtr triMesh) {
	if (triMesh == NULL)
		return;

	auto targetPair = hitable2idx.find(triMesh);
	if (targetPair != hitable2idx.end())
		return;

	Visit(static_cast<const BVH_Node::CPtr>(triMesh));
	sceneData[hitable2idx[triMesh]] = HT_TriMesh;
}

void GenData_HV::Visit(Transform::CPtr transform) {
	if (transform == NULL)
		return;

	auto targetPair = hitable2idx.find(transform);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[transform] = sceneData.size();

	sceneData.push_back(HT_Transform);

	Visit(static_cast<Hitable::CPtr>(transform));

	sceneData.push_back(packData.size() / 4);

	mat4 tfmMat4 = transform->GetTransform();
	for (size_t col = 0; col < 4; col++) {
		vec4 colVec4 = tfmMat4[col];
		for (size_t row = 0; row < 4; row++)
			packData.push_back(colVec4[row]);
	}

	mat4 invTfmMat4 = transform->GetInvTransform();
	for (size_t col = 0; col < 4; col++) {
		vec4 colVec4 = invTfmMat4[col];
		for (size_t row = 0; row < 4; row++)
			packData.push_back(colVec4[row]);
	}

	mat3 normTfmMat3 = transform->GetNormTransform();
	for (size_t col = 0; col < 3; col++) {
		vec3 colVec3 = normTfmMat3[col];
		for (size_t row = 0; row < 3; row++)
			packData.push_back(colVec3[row]);
		packData.push_back(0);
	}

	if (transform->GetChild() == NULL) {
		sceneData.push_back(-float(hitable2idx[transform]));
		return;
	}

	size_t childIt = sceneData.size();
	sceneData.push_back(-1);
	sceneData.push_back(-float(hitable2idx[transform]));

	auto targetChildIdx = hitable2idx.find(transform->GetChild());
	if (targetChildIdx == hitable2idx.end()) {
		sceneData[childIt] = sceneData.size();
		transform->GetChild()->Accept(This());
	}
	else
		sceneData[childIt] = targetChildIdx->second;
}

void GenData_HV::Visit(Volume::CPtr volume) {
	if (volume == NULL)
		return;

	auto targetPair = hitable2idx.find(volume);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[volume] = sceneData.size();

	sceneData.push_back(HT_Volume);

	Visit(static_cast<Hitable::CPtr>(volume));

	sceneData.push_back(volume->GetDensity());

	if (volume->GetBoundary() == NULL) {
		sceneData.push_back(-float(hitable2idx[volume]));
		return;
	}

	size_t childIt = sceneData.size();
	sceneData.push_back(-1);
	sceneData.push_back(-float(hitable2idx[volume]));

	auto targetChildIdx = hitable2idx.find(volume->GetBoundary());
	if (targetChildIdx == hitable2idx.end()) {
		sceneData[childIt] = sceneData.size();
		volume->GetBoundary()->Accept(This());
	}
	else
		sceneData[childIt] = targetChildIdx->second;
}

void GenData_HV::Visit(Sky::CPtr sky) {
	if (sky == NULL)
		return;

	auto targetPair = hitable2idx.find(sky);
	if (targetPair != hitable2idx.end())
		return;

	hitable2idx[sky] = sceneData.size();

	sceneData.push_back(HT_Sky);

	Visit(static_cast<Hitable::CPtr>(sky));
}

void GenData_HV::Visit(Model::CPtr model) {
	Visit(static_cast<const TriMesh::CPtr>(model));
}

void GenData_HV::SetMat(const MatIdxMap & mat2idx) {
	for (auto const & pair : mat2idx) {
		auto target = mat2idxVec.find(pair.first);
		if (target != mat2idxVec.end()) {
			for (auto const & idx : target->second)
				sceneData[idx] = pair.second;
		}
	}
}

void GenData_HV::Accept(GenData_MV::Ptr genFS_MV) {
	for (auto const & pair : mat2idxVec)
		pair.first->Accept(genFS_MV);

	SetMat(genFS_MV->GetMatIdxMap());
}