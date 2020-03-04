#include <3rdParty/HEMesh/HEMesh.h>
#include <CppUtil/Basic/Ptr.h>
#include <iostream>
#include <string>

using namespace CppUtil::Basic;
using namespace std;
using namespace CppUtil;
using namespace Ubpa;

class V;
class E;
class P;
class V : public TVertex<V,E,P> {
public:
	V(const string & name = "NO_NAME") : name(name){}
	~V() { printf("%s dead\n", name.c_str()); }
public:
	string name;
};
class E : public TEdge<V, E, P> {
public:
	E(const string& pre = "E") : pre(pre){}
	~E() { printf("%s dead\n", pre.c_str()); }
public:
	const string Name() const { return "[" + pre + "]" + HalfEdge()->Origin()->name + "-" + HalfEdge()->End()->name; }
private:
	string pre;
};
class P : public TPolygon<V, E, P> {
public:
	P(const string& pre = "P") :pre(pre) {}
	~P() { printf("%s dead\n", pre.c_str()); }
public:
	const string Name() const {
		string name = "[" + pre + "]";
		auto he = HalfEdge();
		name += he->Origin()->name;
		do {
			name += "-" + he->End()->name;
			he = he->Next();
		} while (he != HalfEdge());
		return name;
	}
private:
	string pre;
};

ostream & operator<< (ostream & os, V* v) {
	os << v->name;
	return os;
}

ostream & operator<< (ostream & os, HEMesh<V>::HE* he) {
	os << he->Origin() << "->" << he->End();
	return os;
}

void Print(Ptr<HEMesh<V>> mesh) {
	cout << (mesh->IsValid() ? "[valid]" : "[not valid]") << endl;
	cout << " V:" << mesh->Vertices().size() << endl;
	for (auto v : mesh->Vertices())
		cout << "    "<< v->name << endl;

	cout << "HE:" << mesh->HalfEdges().size() << endl;
	for (auto he : mesh->HalfEdges())
		cout << "    " << he << endl;

	cout << " E:" << mesh->Edges().size() << endl;
	for (auto e : mesh->Edges())
		cout << "    " << e->Name() << endl;

	cout << " P:" << mesh->Polygons().size() << endl << endl;
	for (auto p : mesh->Polygons())
		cout << "    " << p->Name() << endl;

	cout << " B:" << mesh->NumBoundaries() << endl;
	for (auto b : mesh->Boundaries()) {
		cout << "    ";
		for (auto he : b)
			cout << he->Origin() << "-";
		cout << b.back()->End() << endl;
	}
	cout << endl;
}

int main() {
	// test basic
	{
		cout
			<< "------------------" << endl
			<< "    test basic    " << endl
			<< "------------------" << endl;

		auto mesh = make_shared<HEMesh<V>>();
		cout << "add v0, v1, v2" << endl;

		auto v0 = mesh->AddVertex("v0");
		auto v1 = mesh->AddVertex("v1");
		auto v2 = mesh->AddVertex("v2");
		Print(mesh);
		cout << "add e01, e12, e21" << endl;
		auto e01 = mesh->AddEdge(v0, v1, "E0");
		auto e12 = mesh->AddEdge(v1, v2, "E1");
		auto e20 = mesh->AddEdge(v2, v0, "E2");
		Print(mesh);

		cout << "add p0" << endl;
		auto p0 = mesh->AddPolygon({ e01->HalfEdge(),e12->HalfEdge(),e20->HalfEdge() }, "P0");
		Print(mesh);

		cout << e01->Name() << " is " << (e01->IsBoundary() ? "" : "not ") << "a boundary" << endl;
		cout << v0->name << " is " << (v0->IsBoundary() ? "" : "not ") << "a boundary" << endl;
		cout << "remove p0" << endl;
		mesh->RemovePolygon(p0);
		Print(mesh);

		cout << "remove e01" << endl;
		mesh->RemoveEdge(e01);
		Print(mesh);

		cout << "remove v0" << endl;
		mesh->RemoveVertex(v0);
		Print(mesh);
	}
	
	// test spilt edge
	{
		cout
			<< "----------------------" << endl
			<< "    test SpiltEdge    " << endl
			<< "----------------------" << endl;

		auto mesh = make_shared<HEMesh<V>>();

		auto v0 = mesh->AddVertex("v0");
		auto v1 = mesh->AddVertex("v1");
		auto v2 = mesh->AddVertex("v2");
		auto v3 = mesh->AddVertex("v3");

		auto e01 = mesh->AddEdge(v0, v1, "E0");
		auto e12 = mesh->AddEdge(v1, v2, "E1");
		auto e02 = mesh->AddEdge(v0, v2, "E2");
		auto e23 = mesh->AddEdge(v2, v3, "E3");
		auto e30 = mesh->AddEdge(v3, v0, "E4");

		auto he01 = e01->HalfEdge();
		auto he12 = e12->HalfEdge();
		auto he02 = e02->HalfEdge();
		auto he23 = e23->HalfEdge();
		auto he30 = e30->HalfEdge();
		auto he20 = he02->Pair();

		mesh->AddPolygon({ he01,he12,he20 }, "P0");
		mesh->AddPolygon({ he02,he23,he30 }, "P1");

		auto v4 = mesh->SpiltEdge(e02, "v4");
		auto v5 = mesh->SpiltEdge(e01, "v5");

		Print(mesh);
	}

	// test rotate edge
	{
		cout
			<< "-----------------------" << endl
			<< "    test FlipEdge    " << endl
			<< "-----------------------" << endl;

		auto mesh = make_shared<HEMesh<V>>();

		auto v0 = mesh->AddVertex("v1");
		auto v1 = mesh->AddVertex("v2");
		auto v2 = mesh->AddVertex("v3");
		auto v3 = mesh->AddVertex("v4");

		auto e01 = mesh->AddEdge(v0, v1, "E0");
		auto e12 = mesh->AddEdge(v1, v2, "E1");
		auto e02 = mesh->AddEdge(v0, v2, "E2");
		auto e23 = mesh->AddEdge(v2, v3, "E3");
		auto e30 = mesh->AddEdge(v3, v0, "E4");

		auto he01 = e01->HalfEdge();
		auto he12 = e12->HalfEdge();
		auto he02 = e02->HalfEdge();
		auto he23 = e23->HalfEdge();
		auto he30 = e30->HalfEdge();
		auto he20 = he02->Pair();

		mesh->AddPolygon({ he01,he12,he20 }, "P0");
		mesh->AddPolygon({ he02,he23,he30 }, "P1");

		Print(mesh);
		mesh->FlipEdge(e02);
		Print(mesh);
	}
	
	// test collapse edge
	{
		cout
			<< "-------------------------" << endl
			<< "    test CollapseEdge    " << endl
			<< "-------------------------" << endl;

		auto mesh = make_shared<HEMesh<V>>();

		auto v0 = mesh->AddVertex("v0");
		auto v1 = mesh->AddVertex("v1");
		auto v2 = mesh->AddVertex("v2");
		auto v3 = mesh->AddVertex("v3");

		auto e01 = mesh->AddEdge(v0, v1, "E0");
		auto e12 = mesh->AddEdge(v1, v2, "E1");
		auto e02 = mesh->AddEdge(v0, v2, "E2");
		auto e23 = mesh->AddEdge(v2, v3, "E3");
		auto e30 = mesh->AddEdge(v3, v0, "E4");

		auto he01 = e01->HalfEdge();
		auto he12 = e12->HalfEdge();
		auto he02 = e02->HalfEdge();
		auto he23 = e23->HalfEdge();
		auto he30 = e30->HalfEdge();
		auto he20 = he02->Pair();

		mesh->AddPolygon({ he01,he12,he20 }, "P0");
		mesh->AddPolygon({ he02,he23,he30 }, "P1");

		auto v4 = mesh->SpiltEdge(e02);
		v4->name = "v4";

		auto v5 = mesh->SpiltEdge(v4->EdgeWith(v3));
		v5->name = "v5";

		Print(mesh);
	
		auto v6 = mesh->CollapseEdge(v4->EdgeWith(v5));
		v6->name = "v6";

		Print(mesh);
	}

	// test collapse edge 2
	{
		cout
			<< "---------------------------" << endl
			<< "    test CollapseEdge 2    " << endl
			<< "---------------------------" << endl;

		vector<vector<size_t>> indices;
		indices.push_back({ 4, 5, 6, 17, 16 });
		indices.push_back({ 10, 11, 12, 13, 14, 17 });
		indices.push_back({ 14, 15, 16 });
		indices.push_back({ 15, 0, 1, 16 });
		indices.push_back({ 6, 7, 8, 17 });
		indices.push_back({ 8, 9, 17 });
		indices.push_back({ 9, 10, 17 });
		indices.push_back({ 1, 2, 3, 4, 16 });
		indices.push_back({ 14, 16, 17 });
		auto mesh = make_shared<HEMesh<V>>();
		mesh->Init(indices);
		for (size_t i = 0; i <= 17; i++)
			mesh->Vertices().at(i)->name = "v" + to_string(i);
		Print(mesh);
		auto e = V::EdgeBetween(mesh->Vertices().at(16), mesh->Vertices().at(17));
		auto v = mesh->CollapseEdge(e);
		v->name = "v18";
		Print(mesh);
	}

	// test collapse edge 4
	{
		cout
			<< "---------------------------" << endl
			<< "    test CollapseEdge 4    " << endl
			<< "---------------------------" << endl;

		vector<vector<size_t>> indices;
		indices.push_back({ 0, 1, 6 });
		indices.push_back({ 1, 2, 6 });
		indices.push_back({ 2, 7, 6 });
		indices.push_back({ 2, 8, 7 });
		indices.push_back({ 6, 7, 8 });
		indices.push_back({ 2, 3, 8 });

		indices.push_back({ 3, 4, 8 });
		indices.push_back({ 4, 5, 8 });
		indices.push_back({ 5, 9, 8 });
		indices.push_back({ 5, 6, 9 });
		indices.push_back({ 6, 8, 9 });
		indices.push_back({ 5, 0, 6 });

		auto mesh = make_shared<HEMesh<V>>();
		mesh->Init(indices);
		for (size_t i = 0; i <= 9; i++)
			mesh->Vertices().at(i)->name = "v" + to_string(i);
		Print(mesh);
		auto e = V::EdgeBetween(mesh->Vertices().at(6), mesh->Vertices().at(8));
		auto v = mesh->CollapseEdge(e);
		if (v != nullptr) // v should be nullptr
			printf("v should be nullptr\n");
		Print(mesh);
	}

	{
		auto mesh0 = make_shared<HEMesh<>>();
		auto mesh1 = make_shared<HEMesh<V>>(); // V E P
		class tV;
		class tE;
		class tV : public TVertex<tV, tE> {};
		class tE : public TEdge<tV, tE> {};
		auto mesh2 = make_shared<HEMesh<tV>>();
	}

	return 0;
}
