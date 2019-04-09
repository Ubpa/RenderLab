#ifndef _BASIC_HEADER_PTR_H_
#define _BASIC_HEADER_PTR_H_

#include <memory>
#include <typeinfo>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		using Ptr = std::shared_ptr<T>;

		template<typename T>
		using PtrC = Ptr<const T>;

		template<typename T>
		using WPtr = std::weak_ptr<T>;

		template<typename T>
		bool operator<(const WPtr<T> & lhs, const WPtr<T> & rhs) {
			return lhs.lock() < rhs.lock();
		}

		template<typename T>
		using WPtrC = WPtr<const T>;

		template<typename FromType, typename ToType, bool isUp, bool isDown>
		struct CastWrapper;
		template<typename FromType, typename ToType>
		struct CastWrapper<FromType, ToType, true, false> {
			static const Ptr<ToType> call(const Ptr<FromType> & ptr) {
				return std::dynamic_pointer_cast<ToType>(ptr);
			}
			static const WPtr<ToType> call(const WPtr<FromType> & ptr) {
				return std::dynamic_pointer_cast<ToType>(ptr);
			}
		};
		template<typename FromType, typename ToType>
		struct CastWrapper<FromType, ToType, false, true> {
			static const Ptr<ToType> call(const Ptr<FromType> & ptr) {
				return std::static_pointer_cast<ToType>(ptr);
			}
			static const WPtr<ToType> call(const WPtr<FromType> & ptr) {
				return std::static_pointer_cast<ToType>(ptr);
			}
		};
		template<typename FromType, typename ToType>
		struct CastWrapper<FromType, ToType, true, true> {
			static const Ptr<ToType> call(const Ptr<FromType> & ptr) {
				return ptr;
			}
			static const WPtr<ToType> call(const WPtr<FromType> & ptr) {
				return ptr;
			}
		};
		template<typename FromType, typename ToType>
		struct CastWrapper<FromType, ToType, false, false> {
			static const Ptr<ToType> call(const Ptr<FromType> & ptr) {
				return ERROR_FromType_can_not_cast_to_ToType();
			}
			static const WPtr<ToType> call(const WPtr<FromType> & ptr) {
				return ERROR_FromType_can_not_cast_to_ToType();
			}
		};

		template<typename ToType, typename FromType>
		const Ptr<ToType> CastTo(const Ptr<FromType> & ptr) {
			return CastWrapper<FromType, ToType, std::is_base_of_v<FromType, ToType>, std::is_base_of_v<ToType, FromType>>::call(ptr);
		}
	}
}

#endif // !_BASIC_HEADER_PTR_H_
