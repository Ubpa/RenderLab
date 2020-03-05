#pragma once

#include <UGM/vec.h>
#include <vector>

namespace Ubpa {
	namespace Geometry {
		template<typename T>
		bool IsConvexPolygon(const std::vector<point<T, 3>>& points) {
			assert(points.size() >= 4);
			std::vector<vec<T, 3>> vecs(points.size());
			std::vector<vec<T, 3>> norms(points.size());
			for (size_t i = 0; i < points.size(); i++) {
				size_t next = (i + 1) % points.size();
				vecs[i] = points[next] - points[i];
			}
			for (size_t i = 0; i < vecs.size(); i++) {
				size_t next = (i + 1) % points.size();
				norms[i] = vecs[i].cross(vecs[next]);
				for (size_t j = 0; j < i; j++) {
					if (norms[j].dot(norms[i]) < static_cast<T>(0))
						return false;
				}
			}
			return true;
		}
	}
}