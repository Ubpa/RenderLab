#ifndef _CPPUTIL_BASIC_MATH_UGM_FRAME_H_
#define _CPPUTIL_BASIC_MATH_UGM_FRAME_H_

#include <UGM/point.h>
#include <UGM/vec.h>

namespace CppUtil {
	namespace Basic {
		template <typename T>
		class Frame {
		public:
			Frame() {
				T minNum = std::numeric_limits<T>::lowest();
				T maxNum = std::numeric_limits<T>::max();
				minP = Ubpa::point<T, 2>(maxNum, maxNum);
				maxP = Ubpa::point<T, 2>(minNum, minNum);
			}
			Frame(const Ubpa::point<T, 2> & minP, const Ubpa::point<T, 2> & maxP)
				: minP(minP), maxP(maxP) { }
			Frame(const Ubpa::point<T, 2> & p0, const Ubpa::point<T, 2> & p1, std::nullptr_t needPreprocess)
				: minP(Ubpa::point<T, 2>::Min(p0, p1)), maxP(Ubpa::point<T, 2>::Max(p0, p1)) { }
			Frame(const Frame & frame) : minP(frame.minP), maxP(frame.maxP) { }

		public:
			const Ubpa::point<T, 2> & operator[](int idx) const {
				assert(idx == 0 || idx == 1);
				return _data[idx];
			}
			Ubpa::point<T, 2> & operator[](int idx) {
				assert(idx == 0 || idx == 1);
				return _data[idx];
			}

		public:
			bool IsValid() const { return maxP[0] >= minP[0] && maxP[1] >= minP[1]; }
			bool has_nan() const { return minP.has_nan() || maxP.has_nan(); }
			const Ubpa::point<T, 2> Corner(int i) const {
				assert(i >= 0 && i < 4);
				return Ubpa::point<T, 2>(
					_data[(i & 1)][0],
					_data[(i & 2) ? 1 : 0][1]);
			}

			static const Frame Intersect(const Frame & lhs, const Frame & rhs) {
				const auto minP = Ubpa::point<T, 2>::Max(lhs.minP, rhs.minP);
				const auto maxP = Ubpa::point<T, 2>::Min(lhs.maxP, rhs.maxP);
				return Frame(minP, maxP);
			}
			Frame & IntersectWith(const Frame & rhs) {
				minP = Ubpa::point<T, 2>::Max(lhs.minP, rhs.minP);
				maxP = Ubpa::point<T, 2>::Min(lhs.maxP, rhs.maxP);
				return *this;
			}

			const Vector<2, T> Diagonal() const { return maxP - minP; }

			T Area() const {
				if (!IsValid())
					return static_cast<T>(0);
				const auto d = Diagonal();
				return d[0] * d[1];
			}

		public:
			union
			{
				Ubpa::point<T, 2> _data[2];
				struct { Ubpa::point<T, 2> minP, maxP; };
			};
		};
	}

	using Framei = Basic::Frame<int>;
	using Framef = Basic::Frame<float>;
}

#endif // !_CPPUTIL_BASIC_MATH_UGM_FRAME_H_
