#pragma once
#ifndef _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
#define _CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_

#include <CppUtil/Basic/HEMesh/TVertex.h>
#include <CppUtil/Basic/HEMesh/THalfEdge.h>
#include <CppUtil/Basic/HEMesh/TEdge.h>
#include <CppUtil/Basic/HEMesh/TPolygon.h>

#include <vector>
#include <unordered_set>

namespace CppUtil {
	namespace Basic {
		template<typename V, typename E, typename P//,
			//typename = std::enable_if_t<std::is_base_of_v<TVertex<V,E,P>, V>>,
			//typename = std::enable_if_t<std::is_base_of_v<TEdge<V,E,P>, E>>,
			//typename = std::enable_if_t<std::is_base_of_v<TPolygon<V,E,P>, P>>
		>
		class HEMesh : public HeapObj {
		public:
			using HE = THalfEdge<V, E, P>;
		public:
			HEMesh() = default;

		public:
			static const Ptr<HEMesh> New() { return Basic::New<HEMesh>(); }

		public:
			const std::unordered_set<Ptr<V>> & Vertices() { return vertices; }
			const std::unordered_set<Ptr<HE>> & HalfEdges() { return halfEdges; }
			const std::unordered_set<Ptr<E>> & Edges() { return edges; }
			const std::unordered_set<Ptr<P>> & Polygons() { return polygons; }

			static const std::vector<Ptr<HE>> OutHEs(Ptr<V> v) {
				assert(v != nullptr);
				std::vector<Ptr<HE>> hes;
				if (v->IsIsolated())
					return hes;

				auto he = v->HalfEdge();
				do {
					hes.push_back(he);
					he = he->RotateNext();
				} while (he != v->HalfEdge());
				return hes;
			}

			static const std::vector<Ptr<E>> AdjEdges(Ptr<V> v) {
				std::vector<Ptr<E>> edges;
				for (auto he : OutHEs(v))
					edges.push_back(he->Edge());
				return edges;
			}

			static const Ptr<HE> FindFreeOutHE(Ptr<V> v) {
				assert(v != nullptr);
				for (auto outHE : OutHEs(v)) {
					if (outHE->IsFree())
						return outHE;
				}
				return nullptr;
			}

			static const std::vector<Ptr<V>> AdjVertices(Ptr<V> v) {
				assert(v != nullptr);
				std::vector<Ptr<V>> adjVs;
				for(auto he : OutHEs(v))
					adjVs.push_back(CastTo<V>(he->End()));
				return adjVs;
			}

			static bool IsConnected(Ptr<V> v0, Ptr<V> v1) {
				assert(v0 != nullptr && v1 != nullptr);
				for(auto adjV : AdjVertices(v0)) {
					if (adjV == v1)
						return true;
				}
				return false;
			}

			static const std::vector<Ptr<HE>> BoundaryHEs(Ptr<P> polygon) {
				assert(polygon != nullptr);
				std::vector<Ptr<HE>> hes;
				auto he = polygon->HalfEdge();
				do {
					hes.push_back(he);
					he = he->Next();
				} while (he != polygon->HalfEdge());
				return hes;
			}

			const Ptr<V> AddVertex() {
				auto v = Basic::New<V>();
				vertices.insert(v);
				return v;
			}

			const Ptr<E> AddEdge(Ptr<V> v0, Ptr<V> v1) {
				if (v0 == v1) {
					printf("ERROR::HEMesh::AddEdge\n"
						"\t""v0 == v1\n");
					return nullptr;
				}
				if (IsConnected(v0, v1)) {
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
					auto outV0 = FindFreeOutHE(v0);
					assert(outV0 != nullptr);
					auto inV0 = outV0->Pair();
					
					inV0->SetNext(he0);
					he1->SetNext(outV0);
				}
				else
					v0->SetHalfEdge(he0);

				// [link he1]
				if (!v1->IsIsolated()) {
					auto outV1 = FindFreeOutHE(v1);
					assert(outV1 != nullptr);
					auto inV1 = outV1->Pair();

					inV1->SetNext(he1);
					he0->SetNext(outV1);
				}
				else
					v1->SetHalfEdge(he1);

				return e;
			}

			const Ptr<P> AddPolygon(const std::vector<Ptr<HE>> heLoop) {
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

			void RemovePolygon(Ptr<P> polygon) {
				assert(polygon != nullptr);
				auto hes = BoundaryHEs(polygon);
				for (auto he : hes)
					he->SetPolygon(nullptr);
				polygons.erase(polygon);
			}

			void RemoveEdge(Ptr<E> e) {
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

			void RemoveVertex(Ptr<V> v) {
				for (auto e : AdjEdges(v))
					RemoveEdge(e);
				vertices.erase(v);
			}

		protected:
			virtual ~HEMesh() = default;

		private:
			std::unordered_set<Ptr<V>> vertices;
			std::unordered_set<Ptr<HE>> halfEdges;
			std::unordered_set<Ptr<E>> edges;
			std::unordered_set<Ptr<P>> polygons;
		};
	}
}

#endif // !_CPPUTIL_BASIC_HEMESH_BASE_HEMESH_H_
