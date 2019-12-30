template<typename V, typename E, typename P>
const std::vector<Ptr<THalfEdge<V, E, P>>> TPolygon<V, E, P>::BoundaryHEs(Ptr<HE> begin, Ptr<HE> end) {
	std::vector<Ptr<HE>> hes;
	auto he = begin;
	do {
		hes.push_back(he);
		he = he->Next();
	} while (he != end);
	return hes;
}

template<typename V, typename E, typename P>
const std::vector<Ptr<E>> TPolygon<V, E, P>::BoundaryEdges() {
	std::vector<Ptr<E>> edges;
	for (auto he : BoundaryHEs())
		edges.push_back(he->Edge());
	return edges;
}
