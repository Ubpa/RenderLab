#ifndef _VEC3_H_
#define _VEC3_H_

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Vec3 {
		public:
			Vec3(const T & x, const T & y, const T & z);
			T & X();
			T & Y();
			T & Z();
			T & R();
			T & G();
			T & B();
			T * GetData();
			Vec3 & operator +(const Vec3 & v);
			Vec3 & operator -(const Vec3 & v);
			Vec3 & operator *(float a);
			Vec3 & operator /(float a);
			Vec3 & operator =(const Vec3 & v);
		private:
			T data[3];
		};

		template<typename T>
		Vec3<T>::Vec3(const T & x, const T & y, const T & z) {
			data[0] = x;
			data[1] = y;
			data[2] = z;
		}

		template<typename T>
		T & Vec3<T>::X() {
			return data[0];
		}
		template<typename T>
		T & Vec3<T>::Y() {
			return data[1];
		}
		template<typename T>
		T & Vec3<T>::Z() {
			return data[2];
		}
		template<typename T>
		T & Vec3<T>::R() {
			return data[0];
		}
		template<typename T>
		T & Vec3<T>::G() {
			return data[1];
		}
		template<typename T>
		T & Vec3<T>::B() {
			return data[2];
		}

		template<typename T>
		Vec3<T> & Vec3<T>::operator + (const Vec3<T> & v) {
			data[0] += v.data[0];
			data[1] += v.data[1];
			data[2] += v.data[2];
			return *this;
		}

		template<typename T>
		Vec3<T> & Vec3<T>::operator - (const Vec3<T> & v) {
			data[0] -= v.data[0];
			data[1] -= v.data[1];
			data[2] -= v.data[2];
			return *this;
		}

		template<typename T>
		Vec3<T> & Vec3<T>::operator * (float a) {
			data[0] *= a;
			data[1] *= a;
			data[2] *= a;
			return *this;

		}

		template<typename T>
		Vec3<T> & Vec3<T>::operator / (float a) {
			data[0] /= a;
			data[1] /= a;
			data[2] /= a;
			return *this;
		}

		template<typename T>
		Vec3<T> & Vec3<T>::operator = (const Vec3<T> & v) {
			data[0] = v.data[0];
			data[1] = v.data[1];
			data[2] = v.data[2];
			return *this;
		}

		template<typename T>
		T * Vec3<T>::GetData() {
			return &(data[0]);
		}
	}
}

#endif // !_VEC3_H_
