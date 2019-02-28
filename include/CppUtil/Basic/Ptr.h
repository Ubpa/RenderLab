#ifndef _BASIC_HEADER_PTR_H_
#define _BASIC_HEADER_PTR_H_

#include <memory>
#include <typeinfo>

namespace CppUtil {
	namespace Basic {
		//class HeapObj;

		template<typename T>
		using ptr = std::shared_ptr<T>;

		template<typename T>
		struct Has_InitAfterGenSharePtr {
			template<typename U, void (U::*)()> struct HELPS;
			template<typename U> static char Test(HELPS<U, &U::InitAfterGenSharePtr>*);
			template<typename U> static int Test(...);
			const static bool value = sizeof(Test<T>(0)) == sizeof(char);
		};

		template <typename T, bool hasInit>
		struct InitWrapper {};

		template <typename T>
		struct InitWrapper<T, true> {
			inline static void Func(T * p) {
				if (p != nullptr) p->InitAfterGenSharePtr();
			}
		};

		template <typename T>
		struct InitWrapper<T, false> {
			inline static void Func(T * p) { }
		};

		template <typename T>
		struct Init : public InitWrapper<T, Has_InitAfterGenSharePtr<T>::value> { };

		template <typename T1, typename T2, bool castable>
		struct CastWrapper;
		template <typename T1, typename T2>
		struct CastWrapper<T1, T2, true> {
			inline static std::shared_ptr<T1> call(std::shared_ptr<T2> ptr) {
				return std::dynamic_pointer_cast<T1>(ptr);
			}
		};
		template <typename T1, typename T2>
		struct CastWrapper<T1, T2, false> {
			inline static std::shared_ptr<T1> call(std::shared_ptr<T2> ptr) {
				return ERROR_T1_is_not_base_of_T2();
			}
		};
		template <typename T1, typename T2>
		struct Cast : public CastWrapper<T1, T2, std::is_base_of<T1, T2>::value || std::is_base_of<T2, T1>::value> { };

		template<typename T>
		class Ptr : public std::shared_ptr<T> {
		public:
			Ptr(std::nullptr_t p = nullptr) : std::shared_ptr<T>(p) {
				Init<T>::Func(p);
			}

			// T ²»ÊÇ HeapObj
			explicit Ptr(T * p) : std::shared_ptr<T>(p) { }

			template<typename BaseT>
			Ptr(T * p, void(*delFunc)(const BaseT *)) : std::shared_ptr<T>(p, delFunc) {
				Init<T>::Func(p);
			}

			Ptr(const std::shared_ptr<T> & p) : std::shared_ptr<T>(p) { }
			Ptr(const std::shared_ptr<T> && p) : std::shared_ptr<T>(p) { }

			template<typename ChildT>
			Ptr(const Ptr<ChildT> & ptr) : std::shared_ptr<T>(CastWrapper<T, ChildT, std::is_base_of_v<T, ChildT>>::call(ptr)) { }

			template<typename BaseT>
			inline static Ptr<T> Cast(const Ptr<BaseT> & ptr) {
				return Ptr<T>(CastWrapper<T, BaseT, std::is_base_of_v<BaseT, T>>::call(ptr));
			}

			Ptr(const Ptr<std::remove_const_t<T>> & ptr) : std::shared_ptr<T>(ptr) {}

			operator std::shared_ptr<T>() { return std::shared_ptr<T>(*this); }

			inline operator bool() const{
				return *this != nullptr;
			}

			template<typename T>
			inline bool IsPtrOf() const {
				return typeid(*this) == typeid(T);
			}
		};


		template<typename T>
		class WPtr : public std::weak_ptr<T> {
		public:
			using std::weak_ptr<T>::weak_ptr;

			inline bool operator < (const std::weak_ptr<T> & rhs) const {
				return this->lock() < rhs.lock();
			}
		};

		template<typename T>
		using CPtr = Ptr<const T>;
	}
}

#endif // !_BASIC_HEADER_PTR_H_
