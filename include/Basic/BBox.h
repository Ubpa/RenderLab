#ifndef _CPPUTIL_BASIC_MATH_BBOX_H_
#define _CPPUTIL_BASIC_MATH_BBOX_H_

#include <UGM/point.h>
#include <UGM/vec.h>

#include <Basic/Math.h>

#include <limits>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class BBox {
		public:
			BBox() {
				T minNum = std::numeric_limits<T>::lowest();
				T maxNum = std::numeric_limits<T>::max();
				minP = Ubpa::point<T, 3>(maxNum, maxNum, maxNum);
				maxP = Ubpa::point<T, 3>(minNum, minNum, minNum);
			}
			BBox(const Ubpa::point<T, 3> & minP, const Ubpa::point<T, 3> & maxP)
				: minP(minP), maxP(maxP) { }
			BBox(const Ubpa::point<T, 3> & p0, const Ubpa::point<T, 3> & p1, std::nullptr_t needPreprocess)
				: minP(Ubpa::point<T, 3>::Min(p0,p1)), maxP(Ubpa::point<T, 3>::Max(p0, p1)) { }
			BBox(const BBox & bbox) : minP(bbox.minP), maxP(bbox.maxP) { }

		public:
			const Ubpa::point<T, 3> & operator[](int idx) const {
				assert(idx == 0 || idx == 1);
				return data[idx];
			}
			Ubpa::point<T, 3> & operator[](int idx) {
				assert(idx == 0 || idx == 1);
				return data[idx];
			}

		public:
			bool IsValid() const { return maxP[0] >= minP[0] && maxP[1] >= minP[1] && maxP[2] >= minP[2]; }
			bool has_nan() const { return minP.has_nan() || maxP.has_nan(); }
			const Ubpa::point<T, 3> Corner(int i) const {
				assert(i >= 0 && i < 8);
				return Ubpa::point<T, 3>(
					_data[(i & 1)][0],
					_data[(i & 2) ? 1 : 0][1],
					_data[(i & 4) ? 1 : 0][2]);
			}
			const Ubpa::point<T, 3> Center() const {
				return Ubpa::point<T, 3>::Mid(minP, maxP);
			}

			const Ubpa::vec<T, 3> Diagonal() const { return maxP - minP; }
			const T SurfaceArea() const {
				if (!IsValid())
					return static_cast<T>(0);
				const auto d = Diagonal();
				return static_cast<T>(2)*(d[0]*d[1] + d[0]*d[2] + d[1]*d[2]);
			}
			T Volume() const {
				if (!IsValid())
					return static_cast<T>(0);
				const auto d = Diagonal();
				return d[0] * d[1] * d
			}
			int MaxExtent() const {
				const auto d = Diagonal();
				if (d[0] > d[1]) {
					if (d[0] > d[2])
						return 0;
					else
						return 2;
				}
				else if (d[1] > d[0])
					return 1;
				else
					return 2;
			}

			template<typename U>
			const Ubpa::point<T, 3> Lerp(const Val<3, U> & t) {
				return Ubpa::point<T, 3>(
					Math::Lerp(minP[0], maxP[0], t[0]),
					Math::Lerp(minP[1], maxP[1], t[1]),
					Math::Lerp(minP[2], maxP[2], t[2])
				);
			}

			const Ubpa::vec<T, 3> Offset(const Ubpa::point<T, 3> & p) const {
				Ubpa::vec<T, 3> o = p - pMin;
				const auto d = Diagonal();
				o[0] /= d[0];
				o[1] /= d[1];
				o[2] /= d[2];
				return o;
			}

			template <typename U>
			explicit operator BBox<U>() const {
				return BBox<U>((Point<U>)minP, (Point<U>)maxP);
			}

			const BBox Union(const BBox & rhs) const {
				return BBox(Ubpa::point<T, 3>::Min(minP, rhs.minP), Ubpa::point<T, 3>::Max(maxP, rhs.maxP));
			}
			BBox & UnionWith(const BBox & rhs) {
				minP = Ubpa::point<T, 3>::Min(minP, rhs.minP);
				maxP = Ubpa::point<T, 3>::Max(maxP, rhs.maxP);
				return *this;
			}
			const BBox Union(const Ubpa::point<T, 3> & p) const {
				return BBox(minP.MinWith(p), maxP.MaxWith(p));
			}
			BBox & UnionWith(const Ubpa::point<T, 3> & p) {
				minP = Ubpa::point<T, 3>::Min(minP, p);
				maxP = Ubpa::point<T, 3>::Max(maxP, p);
				return *this;
			}

			static const BBox Intersect(const BBox & lhs, const BBox & rhs) {
				const auto minP = Ubpa::point<T, 3>::Max(lhs.minP, rhs.minP);
				const auto maxP = Ubpa::point<T, 3>::Min(lhs.maxP, rhs.maxP);
				return BBox(minP, maxP);
			}
			BBox & IntersectWith(const BBox & rhs) {
				minP = Ubpa::point<T, 3>::Max(lhs.minP, rhs.minP);
				maxP = Ubpa::point<T, 3>::Min(lhs.maxP, rhs.maxP);
				return *this;
			}

		public:
			bool operator ==(const BBox & rhs) {
				return minP == rhs.minP && maxP == rhs.maxP;
			}

			bool operator !=(const BBox & rhs) {
				return minP != rhs.minP || maxP != rhs.maxP;
			}

			BBox & operator=(const BBox & rhs) {
				minP = rhs.minP;
				maxP = rhs.maxP;
				return *this;
			}

			friend std::ostream &operator<<(std::ostream &os, const BBox &b) {
				os << "[ " << b.minP << " - " << b.maxP << " ]";
				return os;
			}

		public:
			union
			{
				Ubpa::point<T, 3> _data[2];
				struct
				{
					Ubpa::point<T, 3> minP, maxP;
				};
			};
		};
	}

	using BBoxf = Basic::BBox<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_BBOX_H_
