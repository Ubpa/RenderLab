#ifndef _CPPUTIL_BASIC_MATH_BBOX_H_
#define _CPPUTIL_BASIC_MATH_BBOX_H_

#include <CppUtil/Basic/Point.h>
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
				minP = Point<T>(maxNum, maxNum, maxNum);
				maxP = Point<T>(minNum, minNum, minNum);
			}
			BBox(const Point<T> & minP, const Point<T> & maxP)
				: minP(minP), maxP(maxP) { }
			BBox(const Point<T> & p0, const Point<T> & p1, std::nullptr_t needPreprocess)
				: minP(Point<T>::Min(p0,p1)), maxP(Point<T>::Max(p0, p1)) { }
			BBox(const BBox & bbox) : minP(bbox.minP), maxP(bbox.maxP) { }

		public:
			const Point<T> & operator[](int zeroOrOne) const { return data[zeroOrOne]; }
			Point<T> & operator[](int zeroOrOne) { return data[zeroOrOne]; }

		public:
			bool HasNaN() const { return minP.HasNaN() || maxP.HasNaN(); }
			const Point<T> Corner(int i) const {
				return Point<T>(
					_data[(i & 1)].x,
					_data[(i & 2)].y,
					_data[(i & 4)].z);
			}
			const Point<T> Center() const {
				return Point<T>::Mid(minP, maxP);
			}

			const Vector<T> Diagonal() const { return maxP - minP; }
			const T SurfaceArea() const {
				const auto d = Diagonal();
				return static_cast<T>(2)*(d.x*d.y + d.x*d.z + d.y*d.z);
			}
			T Volume() const {
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
			const Point<T> Lerp(const Val3<U> & t) {
				return Point<T>(
					Math::Lerp(minP.x, maxP.x, t.x),
					Math::Lerp(minP.y, maxP.y, t.y),
					Math::Lerp(minP.z, maxP.z, t.z)
				);
			}

			const Vector<T> Offset(const Point<T> & p) const {
				Vector<T> o = p - pMin;
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
				return BBox(Point<T>::Min(minP, rhs.minP), Point<T>::Max(maxP, rhs.maxP));
			}
			BBox & UnionWith(const BBox & rhs) {
				minP = Point<T>::Min(minP, rhs.minP);
				maxP = Point<T>::Max(maxP, rhs.maxP);
				return *this;
			}
			const BBox Union(const Point<T> & p) const {
				const auto minP = Point<T>::Min(minP, p);
				const auto maxP = Point<T>::Max(maxP, p);
				return BBox(minP, maxP);
			}
			BBox & UnionWith(const Point<T> & p) {
				minP = Point<T>::Min(minP, p);
				maxP = Point<T>::Max(maxP, p);
				return *this;
			}

			static const BBox Intersect(const BBox & lhs, const BBox & rhs) {
				const auto minP = Point<T>::Max(lhs.minP, rhs.minP);
				const auto maxP = Point<T>::Min(lhs.maxP, rhs.maxP);
				return BBox(minP, maxP);
			}
			BBox & IntersectWith(const BBox & rhs) {
				minP = Point<T>::Max(lhs.minP, rhs.minP);
				maxP = Point<T>::Min(lhs.maxP, rhs.maxP);
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
				Point<T> _data[2];
				struct
				{
					Point<T> minP, maxP;
				};
			};
		};
	}

	using BBoxf = Basic::BBox<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_BBOX_H_
