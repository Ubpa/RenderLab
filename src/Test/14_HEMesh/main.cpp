#include <CppUtil/Basic/HEMesh/HEMesh.h>
#include <CppUtil/Basic/UGM/UGM.h>
#include <iostream>
#include <string>

using namespace CppUtil::Basic;
using namespace std;
using namespace CppUtil;

class V : public BaseVertex {
protected:
	virtual ~V() = default;
public:
	string name;
};

ostream & operator<< (ostream & os, Ptr<BaseVertex> v) {
	os << CastTo<V>(v)->name;
	return os;
}

ostream & operator<< (ostream & os, Ptr<BaseHalfEdge> he) {
	os << he->Origin() << "->" << he->End()
		<< ", next " << he->Next()->Origin() << "->" << he->Next()->End()
		<< ", pair " << he->Pair()->Origin() << "->" << he->Pair()->End();
	return os;
}

ostream & operator<< (ostream & os, Ptr<BaseEdge> e) {
	os << e->HalfEdge()->Origin() << "-" << e->HalfEdge()->End();
	return os;
}

ostream & operator<< (ostream & os, Ptr<BasePolygon> p) {
	auto he = p->HalfEdge();
	cout << he->Origin();
	do {
		cout << "-" << he->End();
		he = he->Next();
	} while (he != p->HalfEdge());
	return os;
}

void Print(Ptr<HEMesh<V>> mesh) {
	cout << " V:" << mesh->Vertices().size() << endl;
	for (auto v : mesh->Vertices())
		cout << "    "<< v->name << endl;

	cout << "HE:" << mesh->HalfEdges().size() << endl;
	for (auto he : mesh->HalfEdges())
		cout << "    " << he << endl;

	cout << " E:" << mesh->Edges().size() << endl;
	for (auto e : mesh->Edges())
		cout << "    " << e << endl;

	cout << " P:" << mesh->Polygons().size() << endl << endl;
	for (auto p : mesh->Polygons())
		cout << "    " << p << endl;

	cout << endl;
}

int main() {
	auto mesh = HEMesh<V>::New();

	cout << "add v0, v1, v2" << endl;
	auto v0 = mesh->AddVertex();
	v0->name = "v0";
	auto v1 = mesh->AddVertex();
	v1->name = "v1";
	auto v2 = mesh->AddVertex();
	v2->name = "v2";
	Print(mesh);

	cout << "add e01, e12, e21" << endl;
	auto e01 = mesh->AddEdge(v0, v1);
	auto e12 = mesh->AddEdge(v1, v2);
	auto e20 = mesh->AddEdge(v2, v0);
	Print(mesh);

	cout << "add p0, p1" << endl;
	auto p0 = mesh->AddPolygon({ e01->HalfEdge(),e12->HalfEdge(),e20->HalfEdge() });
	auto p1 = mesh->AddPolygon({ e20->HalfEdge()->Pair(),e12->HalfEdge()->Pair(),e01->HalfEdge()->Pair() });
	Print(mesh);

	cout << "remove p0" << endl;
	mesh->RemovePolygon(p0);
	Print(mesh);

	cout << "remove e01" << endl;
	mesh->RemoveEdge(e01);
	Print(mesh);

	cout << "remove v0" << endl;
	mesh->RemoveVertex(v0);
	Print(mesh);
	return 0;
}
