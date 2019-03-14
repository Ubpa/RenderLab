#ifndef _CPPUTIL_BASIC_HEADER_ERROR_H_
#define _CPPUTIL_BASIC_HEADER_ERROR_H_

#include <CppUtil/Basic/FunctionTraits.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Error {
		public:
			static const T & Val() {
				static const T value{};
				return value;
			}

		public:
			static bool Is(const T & val) {
				return val == Val();
			}

		private:
			Error() = delete;
			~Error() = delete;
			Error(const Error & error) = delete;
			Error & operator ==(const Error & error) = delete;
		};

		template<typename T>
		bool IsError(const T & val) {
			return Error<T>::Is(val);
		}

		template<typename FuncT>
		const RetT<FuncT> & ErrorRetVal(const FuncT & func) {
			return Error<RetT<FuncT>>::Val();
		}

	}
}

#endif//!_CPPUTIL_BASIC_HEADER_ERROR_H_
