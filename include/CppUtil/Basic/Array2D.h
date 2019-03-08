#ifndef _BASIC_HEADER_ARRAY_2D_H_
#define _BASIC_HEADER_ARRAY_2D_H_

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Array2D {
		public:
			Array2D(uint row, uint col);
			~Array2D();
			//----------
			T & At(uint row, uint col);
			T & operator()(uint row, uint col);
			void Copy(uint row, uint col, uint n, const T * src);
			uint GetRow() const;
			uint GetCol() const;
			uint GetArrSize() const;
			uint GetMemSize() const;
			T * GetData();
		private:
			const uint row;
			const uint col;
			T * data;
		};

		//-------------------------------------------------------------------------

		template<typename T>
		Array2D<T>::Array2D(uint row, uint col)
			:row(row), col(col) {
			data = new T[row*col];
		}

		template<typename T>
		Array2D<T>::~Array2D() {
			delete[] data;
			data = nullptr;
		}

		template<typename T>
		uint Array2D<T>::GetRow() const {
			return row;
		}

		template<typename T>
		uint Array2D<T>::GetCol() const {
			return col;
		}

		template<typename T>
		T & Array2D<T>::At(uint row, uint col) {
			return data[row * this->col + col];
		}

		template<typename T>
		T & Array2D<T>::operator()(uint row, uint col) {
			return data[row * this->col + col];
		}

		template<typename T>
		void Array2D<T>::Copy(uint row, uint col, uint n, const T * src) {
			for (uint i = 0; i < n; i++) {
				At(row, col++) = src[i];
				if (col == this->col) {
					row++;
					col = 0;
				}
			}
		}

		template<typename T>
		uint Array2D<T>::GetArrSize() const {
			return row * col;
		}

		template<typename T>
		uint Array2D<T>::GetMemSize() const {
			return row * col * static_cast<uint>(sizeof(T));
		}

		template<typename T>
		T * Array2D<T>::GetData() {
			return data;
		}
	}
}

#endif // !_BASIC_HEADER_ARRAY_2D_H_
