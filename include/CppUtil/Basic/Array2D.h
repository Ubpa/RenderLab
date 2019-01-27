#ifndef _ARRAY2D_H_
#define _ARRAY2D_H_

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Array2D {
		public:
			Array2D(size_t row, size_t col);
			~Array2D();
			//----------
			T & At(size_t row, size_t col);
			T & operator()(size_t row, size_t col);
			void Copy(size_t row, size_t col, size_t n, const T * src);
			size_t GetRow() const;
			size_t GetCol() const;
			size_t GetArrSize() const;
			size_t GetMemSize() const;
			T * GetData();
		private:
			const size_t row;
			const size_t col;
			T * data;
		};

		//-------------------------------------------------------------------------

		template<typename T>
		Array2D<T>::Array2D(size_t row, size_t col)
			:row(row), col(col) {
			data = new T[row*col];
		}

		template<typename T>
		Array2D<T>::~Array2D() {
			delete[] data;
			data = nullptr;
		}

		template<typename T>
		size_t Array2D<T>::GetRow() const {
			return row;
		}

		template<typename T>
		size_t Array2D<T>::GetCol() const {
			return col;
		}

		template<typename T>
		T & Array2D<T>::At(size_t row, size_t col) {
			return data[row * this->col + col];
		}

		template<typename T>
		T & Array2D<T>::operator()(size_t row, size_t col) {
			return data[row * this->col + col];
		}

		template<typename T>
		void Array2D<T>::Copy(size_t row, size_t col, size_t n, const T * src) {
			for (size_t i = 0; i < n; i++) {
				At(row, col++) = src[i];
				if (col == this->col) {
					row++;
					col = 0;
				}
			}
		}

		template<typename T>
		size_t Array2D<T>::GetArrSize() const {
			return row * col;
		}

		template<typename T>
		size_t Array2D<T>::GetMemSize() const {
			return row * col * sizeof(T);
		}
		template<typename T>
		T * Array2D<T>::GetData() {
			return data;
		}
	}
}

#endif // !_MATRIX_H_
