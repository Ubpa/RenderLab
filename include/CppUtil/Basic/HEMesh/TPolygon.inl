template<typename V, typename E, typename P>
const std::vector<Ptr<THalfEdge<V,E,P>>> TPolygon<V,E,P>::BoundaryHEs() {
	std::vector<Ptr<HE>> hes;
	auto he = HalfEdge();
	do {
		hes.push_back(he);
		he = he->Next();
	} while (he != HalfEdge());
	return hes;
}
