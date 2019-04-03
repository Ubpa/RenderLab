#ifndef _CPPUTIL_BASIC_MATH_BBOX_H_
#define _CPPUTIL_BASIC_MATH_BBOX_H_

#include <CppUtil/Basic/Point3.h>
#include <CppUtil/Basic/Vector3.h>
#include <CppUtil/Basic/Math.h>

#include <limits>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class BBox {
		public:
			BBox() {
				T minNum = std::numeric_limits<T>::lowest();
				T maxNum = std::numeric_limits<T>::max();
				minP = Point<3, T>(maxNum, maxNum, maxNum);
				maxP = Point<3, T>(minNum, minNum, minNum);
			}
			BBox(const Point<3, T> & minP, const Point<3, T> & maxP)
				: minP(minP), maxP(maxP) { }
			BBox(const Point<3, T> & p0, const Point<3, T> & p1, std::nullptr_t needPreprocess)
				: minP(Point<3, T>::Min(p0,p1)), maxP(Point<3, T>::Max(p0, p1)) { }
			BBox(const BBox & bbox) : minP(bbox.minP), maxP(bbox.maxP) { }

		public:
			const Point<3, T> & operator[](int zeroOrOne) const { return data[zeroOrOne]; }
			Point<3, T> & operator[](int zeroOrOne) { return data[zeroOrOne]; }

		public:
			bool IsValid() const { return maxP.x >= minP.x && maxP.y >= minP.y && maxP.z >= minP.z; }
			bool HasNaN() const { return minP.HasNaN() || maxP.HasNaN(); }
			const Point<3, T> Corner(int i) const {
				return Point<3, T>(
					_data[(i & 1)].x,
					_data[(i & 2) ? 1 : 0].y,
					_data[(i & 4) ? 1 : 0].z);
			}
			const Point<3, T> Center() const {
				return Point<3, T>::Mid(minP, maxP);
			}

			const Vector<3, T> Diagonal() const { return maxP - minP; }
			const T SurfaceArea() const {
				if (!IsValid())
					return static_cast<T>(0);
				const auto d = Diagonal();
				return static_cast<T>(2)*(d.x*d.y + d.x*d.z + d.y*d.z);
			}
			T Volume() const {
				if (!IsValid())
					return static_cast<T>(0);
				const auto d = Diagonal();
				return d.x * d.y * d
			}
			int MaxExtent() const {
				const auto d = Diagonal();
				if (d.x > d.y) {
					if (d.x > d.z)
						return 0;
					else
						return 2;
				}
				else if (d.y > d.x)
					return 1;
				else
					return 2;
			}

			template<typename U>
			const Point<3, T> Lerp(const Val<3, U> & t) {
				return Point<3, T>(
					Math::Lerp(minP.x, maxP.x, t.x),
					Math::Lerp(minP.y, maxP.y, t.y),
					Math::Lerp(minP.z, maxP.z, t.z)
				);
			}

			const Vector<3, T> Offset(const Point<3, T> & p) const {
				Vector<3, T> o = p - pMin;
				const auto d = Diagonal();
				o.x /= d.x;
				o.y /= d.y;
				o.z /= d.z;
				return o;
			}

			template <typename U>
			explicit operator BBox<U>() const {
				return BBox<U>((Point<U>)minP, (Point<U>)maxP);
			}

			const BBox Union(const BBox & rhs) const {
				return BBox(Point<3, T>::Min(minP, rhs.minP), Point<3, T>::Max(maxP, rhs.maxP));
			}
			BBox & UnionWith(const BBox & rhs) {
				minP = Point<3, T>::Min(minP, rhs.minP);
				maxP = Point<3, T>::Max(maxP, rhs.maxP);
				return *this;
			}
			const BBox Union(const Point<3, T> & p) const {
				const auto minP = Point<3, T>::Min(minP, p);
				const auto maxP = Point<3, T>::Max(maxP, p);
				return BBox(minP, maxP);
			}
			BBox & UnionWith(const Point<3, T> & p) {
				minP = Point<3, T>::Min(minP, p);
				maxP = Point<3, T>::Max(maxP, p);
				return *this;
			}

			static const BBox Intersect(const BBox & lhs, const BBox & rhs) {
				const auto minP = Point<3, T>::Max(lhs.minP, rhs.minP);
				const auto maxP = Point<3, T>::Min(lhs.maxP, rhs.maxP);
				return BBox(minP, maxP);
			}
			BBox & IntersectWith(const BBox & rhs) {
				minP = Point<3, T>::Max(lhs.minP, rhs.minP);
				maxP = Point<3, T>::Min(lhs.maxP, rhs.maxP);
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
				Point<3, T> _data[2];
				struct
				{
					Point<3, T> minP, maxP;
				};
			};
		};
	}

	using BBoxf = Basic::BBox<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_BBOX_H_
