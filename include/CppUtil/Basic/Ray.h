#ifndef _CPPUTIL_BASIC_MATH_RAY_H_
#define _CPPUTIL_BASIC_MATH_RAY_H_

#include <CppUtil/Basic/Point.h>
#include <CppUtil/Basic/Vector.h>

namespace CppUtil {
	namespace Basic {
		class Ray {
		public:
			Ray(const Pointf & o, const Vectorf & d) : o(o), d(d) { }
			
		public:
			bool HasNaN() const { return o.HasNaN() || d.HasNaN(); }
			Pointf operator()(float t) const { return o + t * d; }

			friend std::ostream &operator<<(std::ostream &os, const Ray &r) {
				os << "[o=" << r.o << ", d=" << r.d << "]";
				return os;
			}

		public:
			Pointf o;
			Vectorf d;
		};
	}
}

#endif // !_CPPUTIL_BASIC_MATH_RAY_H_
