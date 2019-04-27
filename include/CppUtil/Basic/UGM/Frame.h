#ifndef _CPPUTIL_BASIC_MATH_UGM_FRAME_H_
#define _CPPUTIL_BASIC_MATH_UGM_FRAME_H_

#include <CppUtil/Basic/UGM/Point2.h>
#include <CppUtil/Basic/UGM/Vector2.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Frame {
		public:
			Frame() {
				T minNum = std::numeric_limits<T>::lowest();
				T maxNum = std::numeric_limits<T>::max();
				minP = Point<2, T>(maxNum, maxNum);
				maxP = Point<2, T>(minNum, minNum);
			}
			Frame(const Point<2, T> & minP, const Point<2, T> & maxP)
				: minP(minP), maxP(maxP) { }
			Frame(const Point<2, T> & p0, const Point<2, T> & p1, std::nullptr_t needPreprocess)
				: minP(Point<2, T>::Min(p0, p1)), maxP(Point<2, T>::Max(p0, p1)) { }
			Frame(const Frame & frame) : minP(frame.minP), maxP(frame.maxP) { }

		public:
			const Point<2, T> & operator[](int idx) const {
				assert(idx == 0 || idx == 1);
				return _data[idx];
			}
			Point<2, T> & operator[](int idx) {
				assert(idx == 0 || idx == 1);
				return _data[idx];
			}

		public:
			bool IsValid() const { return maxP.x >= minP.x && maxP.y >= minP.y; }
			bool HasNaN() const { return minP.HasNaN() || maxP.HasNaN(); }
			const Point<2, T> Corner(int i) const {
				assert(i >= 0 && i < 4);
				return Point<2, T>(
					_data[(i & 1)].x,
					_data[(i & 2) ? 1 : 0].y);
			}

			static const Frame Intersect(const Frame & lhs, const Frame & rhs) {
				const auto minP = Point<2, T>::Max(lhs.minP, rhs.minP);
				const auto maxP = Point<2, T>::Min(lhs.maxP, rhs.maxP);
				return Frame(minP, maxP);
			}
			Frame & IntersectWith(const Frame & rhs) {
				minP = Point<2, T>::Max(lhs.minP, rhs.minP);
				maxP = Point<2, T>::Min(lhs.maxP, rhs.maxP);
				return *this;
			}

			const Vector<2, T> Diagonal() const { return maxP - minP; }

			T Area() const {
				if (!IsValid())
					return static_cast<T>(0);
				const auto d = Diagonal();
				return d.x * d.y;
			}

		public:
			union
			{
				Point<2, T> _data[2];
				struct { Point<2, T> minP, maxP; };
			};
		};
	}

	using Framei = Basic::Frame<int>;
	using Framef = Basic::Frame<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_FRAME_H_
