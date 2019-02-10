#ifndef _PTR_H_
#define _PTR_H_

#include <memory>

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
			static void Func(T * p) {
				if (p != nullptr) p->InitAfterGenSharePtr();
			}
		};

		template <typename T>
		struct InitWrapper<T, false> {
			static void Func(T * p) { }
		};

		template <typename T>
		struct Init : public InitWrapper<T, Has_InitAfterGenSharePtr<T>::value> { };

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
			Ptr(const Ptr<ChildT> & ptr) : std::shared_ptr<T>(std::dynamic_pointer_cast<T>(std::shared_ptr<ChildT>(ptr))) { }

			operator bool() const{
				return *this != nullptr;
			}
		};


		template<typename T>
		class WPtr : public std::weak_ptr<T> {
		public:
			using std::weak_ptr<T>::weak_ptr;

			bool operator < (const std::weak_ptr<T> & rhs) const {
				return this->lock() < rhs.lock();
			}
		};

		template<typename T>
		using CPtr = Ptr<const T>;
	}
}

#endif // !_PTR_H_
