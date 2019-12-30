template<typename V, typename E, typename P>
const Ptr<THalfEdge<V,E,P>> THalfEdge<V, E, P>::Pre() {
	Ptr<THalfEdge> he;
	for (he = This<THalfEdge>(); he->Next()->Next() != This<THalfEdge>(); he = he->Next())
		;// empty
	return he->Next();
}
