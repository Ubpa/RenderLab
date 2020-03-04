#pragma once
#include <Basic/UGM/UGM.h>

namespace CppUtil {
	namespace Basic {
		namespace Geometry {
			template<typename T>
			bool IsConvexPolygon(const std::vector<Point<3, T>>& points) {
				assert(points.size() >= 4);
				std::vector<Vector<3, T>> vecs(points.size());
				std::vector<Vector<3, T>> norms(points.size());
				for (size_t i = 0; i < points.size(); i++) {
					size_t next = (i + 1) % points.size();
					vecs[i] = points[next] - points[i];
				}
				for (size_t i = 0; i < vecs.size(); i++) {
					size_t next = (i + 1) % points.size();
					norms[i] = vecs[i].Cross(vecs[next]);
					for (size_t j = 0; j < i; j++) {
						if (norms[j].Dot(norms[i]) < static_cast<T>(0))
							return false;
					}
				}
				return true;
			}
		}
	}
}