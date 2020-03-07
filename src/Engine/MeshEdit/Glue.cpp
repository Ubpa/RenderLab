#include <Engine/MeshEdit/Glue.h>

#include <Engine/Primitive/TriMesh.h>
#include <map>

using namespace Ubpa;

using namespace std;

bool Glue::Run() {
	if (triMesh == nullptr || triMesh->GetType() == TriMesh::INVALID) {
		printf("ERROR::Glue::Run:\n"
			"\t""triMesh == nullptr || triMesh->GetType() == TriMesh::INVALID\n");
		return false;
	}

	auto & positions = triMesh->GetPositions();
	vector<unsigned> indice;
	vector<pointf3> uniquePos;
	map<pointf3, unsigned> pos2idx;

	for (auto triangle : triMesh->GetTriangles()) {
		for (int i = 0; i < 3; i++) {
			auto pos = positions[triangle->idx[i]];
			auto target = pos2idx.find(pos);
			if (target == pos2idx.end()) {
				pos2idx[pos] = static_cast<unsigned>(uniquePos.size());
				uniquePos.push_back(pos);
				target = pos2idx.find(pos);
			}
			indice.push_back(target->second);
		}
	}
	printf("Glue: %zd -> %zd\n", positions.size(), uniquePos.size());

	triMesh->Init(indice, uniquePos);

	return true;
}
