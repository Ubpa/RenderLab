template<typename V, typename _0, typename _1, typename _2>
const Ptr<V> HEMesh<V, _0, _1, _2>::AddVertex() {
	auto v = Basic::New<V>();
	vertices.insert(v);
	return v;
}

template<typename V, typename _0, typename _1, typename _2>
const Ptr<typename V::E_t> HEMesh<V, _0, _1, _2>::AddEdge(Ptr<V> v0, Ptr<V> v1) {
	if (v0 == v1) {
		printf("ERROR::HEMesh::AddEdge\n"
			"\t""v0 == v1\n");
		return nullptr;
	}
	if (V::IsConnected(v0, v1)) {
		printf("ERROR::HEMesh::AddEdge\n"
			"\t""v0 is already connected with v1\n");
		return nullptr;
	}

	auto e = Basic::New<E>();
	auto he0 = Basic::New<HE>();
	auto he1 = Basic::New<HE>();
	edges.insert(e);
	halfEdges.insert(he0);
	halfEdges.insert(he1);

	// [init]
	e->SetHalfEdge(he0);

	he0->SetNext(he1);
	he0->SetPair(he1);
	he0->SetOrigin(v0);
	he0->SetEdge(e);

	he1->SetNext(he0);
	he1->SetPair(he0);
	he1->SetOrigin(v1);
	he1->SetEdge(e);

	// [link he0]
	if (!v0->IsIsolated()) {
		auto outV0 = v0->FindFreeOutHE();
		assert(outV0 != nullptr);
		auto inV0 = outV0->Pair();

		inV0->SetNext(he0);
		he1->SetNext(outV0);
	}
	else
		v0->SetHalfEdge(he0);

	// [link he1]
	if (!v1->IsIsolated()) {
		auto outV1 = v1->FindFreeOutHE();
		assert(outV1 != nullptr);
		auto inV1 = outV1->Pair();

		inV1->SetNext(he1);
		he0->SetNext(outV1);
	}
	else
		v1->SetHalfEdge(he1);

	return e;
}

template<typename V, typename _0, typename _1, typename _2>
const Ptr<typename V::P_t> HEMesh<V, _0, _1, _2>::AddPolygon(const std::vector<Ptr<HE>> heLoop) {
	if (heLoop.size() <= 1) {
		printf("ERROR::HEMesh::AddPolygon:\n"
			"\t""heLoop's size <= 1\n");
		return nullptr;
	}
	for (int i = 0; i < heLoop.size(); i++) {
		if (!heLoop[i]->IsFree()) {
			printf("ERROR::HEMesh::AddPolygon:\n"
				"\t""heLoop[%d] isn't free\n", i);
			return nullptr;
		}
		int next = (i + 1) % heLoop.size();
		if (heLoop[i]->Next() != heLoop[next]) {
			printf("ERROR::HEMesh::AddPolygon:\n"
				"\t""heLoop[%d]'s next isn't heLoop[%d]\n", i, next);
			return nullptr;
		}
	}

	auto polygon = Basic::New<P>();
	polygons.insert(polygon);

	polygon->SetHalfEdge(heLoop[0]);
	for (auto he : heLoop)
		he->SetPolygon(polygon);

	return polygon;
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::RemovePolygon(Ptr<P> polygon) {
	assert(polygon != nullptr);
	for (auto he : polygon->BoundaryHEs())
		he->SetPolygon(nullptr);
	polygons.erase(polygon);
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::RemoveEdge(Ptr<E> e) {
	assert(e != nullptr);
	auto he0 = e->HalfEdge();
	auto he1 = he0->Pair();

	if (!he0->IsFree())
		RemovePolygon(he0->Polygon());
	if (!he1->IsFree())
		RemovePolygon(he1->Polygon());

	// init
	auto v0 = he0->Origin();
	auto v1 = he1->Origin();
	auto inV0 = he0->Pre();
	auto inV1 = he1->Pre();
	auto outV0 = he0->RotateNext();
	auto outV1 = he1->RotateNext();

	// [link off he0]
	if (v0->HalfEdge() == he0)
		v0->SetHalfEdge(outV0 == he0 ? nullptr : outV0);

	inV0->SetNext(outV0);

	// [link off he1]
	if (v1->HalfEdge() == he1)
		v1->SetHalfEdge(outV1 == he1 ? nullptr : outV1);

	inV1->SetNext(outV1);

	// delete
	halfEdges.erase(he0);
	halfEdges.erase(he1);
	edges.erase(e);
}

template<typename V, typename _0, typename _1, typename _2>
void HEMesh<V, _0, _1, _2>::RemoveVertex(Ptr<V> v) {
	for (auto e : v->AdjEdges())
		RemoveEdge(e);
	vertices.erase(v);
}
