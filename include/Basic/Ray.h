#ifndef _CPPUTIL_BASIC_MATH_UGM_RAY_H_
#define _CPPUTIL_BASIC_MATH_UGM_RAY_H_

#include <UGM/point.h>
#include <UGM/vec.h>
#include <UGM/val.h>

#include <tuple>

namespace CppUtil {
	namespace Basic {
		class Ray {
		public:
			Ray(const Ubpa::pointf3 & o, const Ubpa::vecf3 & d) : o(o), d(d) { }
			
		public:
			bool HasNAN() const { return o.has_nan() || d.has_nan(); }
			const Ubpa::pointf3 operator()(float t) const { return o + t * d; }
			const Ubpa::pointf3 At(float t) const { return o + t * d ; }

			friend std::ostream &operator<<(std::ostream &os, const Ray &r) {
				os << "[o=" << r.o << ", d=" << r.d << "]";
				return os;
			}

			// isIntersect, [w, u, v], t
			inline const std::tuple<bool, Ubpa::valf3, float> IntersectTriangle(const Ubpa::pointf3& v0, const Ubpa::pointf3& v1, const Ubpa::pointf3& v2) const;

		public:
			Ubpa::pointf3 o;
			Ubpa::vecf3 d;
		};

		const std::tuple<bool, Ubpa::valf3, float> Ray::IntersectTriangle(const Ubpa::pointf3& v0, const Ubpa::pointf3& v1, const Ubpa::pointf3& v2) const {
			const auto e1 = v1 - v0;
			const auto e2 = v2 - v0;

			const auto e1_x_d = e1.cross(d);
			const float denominator = e1_x_d.dot(e2);

			if (denominator == 0) // parallel
				return { false, Ubpa::valf3(0.f), 0.f };

			const float inv_denominator = 1.0f / denominator;

			const auto s = o - v0;

			const auto e2_x_s = e2.cross(s);
			const float r1 = e2_x_s.dot(d);
			const float u = r1 * inv_denominator;
			if (u < 0 || u > 1)
				return { false, Ubpa::valf3(0.f), 0.f };

			const float r2 = e1_x_d.dot(s);
			const float v = r2 * inv_denominator;
			if (v < 0 || v > 1)
				return { false, Ubpa::valf3(0.f), 0.f };

			const float u_plus_v = u + v;
			if (u_plus_v > 1)
				return { false, Ubpa::valf3(0.f), 0.f };

			const float r3 = e2_x_s.dot(e1);
			const float t = r3 * inv_denominator;

			if (t < 0)
				return { false, Ubpa::valf3(0.f), 0.f };

			return { true, Ubpa::valf3(1 - u_plus_v, u, v), t };
		}
	}
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_RAY_H_
