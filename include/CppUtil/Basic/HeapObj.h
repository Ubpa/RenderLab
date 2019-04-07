#ifndef _BASIC_HEAP_OBJ_HEAP_OBJ_H_
#define _BASIC_HEAP_OBJ_HEAP_OBJ_H_

#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		// 由于 HeapObj 需要模板
		// 可能需要一个公有的基类，所以提供了一个 HeapObj_Base
		class HeapObj_Base {
		// 将 new 的权限交给了 _New 函数
		template<typename ImplT, typename ...Args>
		friend const CppUtil::Basic::Ptr<ImplT> _New(Args... args);

		protected:
			// 由于构造函数中不可使用 This(), CThis(), WPtr(), WCPtr()
			// 所以专门提供了一个 Init 函数
			// 该函数会在 new 对象，生成 shared_ptr 后调用
			// 所以该函数内部可使用 This(), CThis(), WPtr(), WCPtr()
			virtual void Init() {}

		protected:
			// 这里 protected 构造函数和析构函数
			// 使得用户不能直接在栈上创建 HeapObj_Base 对象
			// 但是还是可以在栈上创建 子类
			// 所以子类也要 protected 构造函数和析构函数

			HeapObj_Base() = default;
			virtual ~HeapObj_Base() = default;

		private:
			// private new 和 delete
			// 这样用户就无法使用 new 了
			// 而且子类也没法 new

			void * operator new(size_t size) {
				if (void *mem = malloc(size))
					return mem;
				else
					throw std::bad_alloc();
			}

			void operator delete(void * mem) noexcept {
				free(mem);
			}
		};

		// 调用 ImplT 的构造函数，然后生成 shared_ptr，然后调用 virtual 的 Init 函数
		template<typename ImplT, typename ...Args>
		const Ptr<ImplT> _New(Args... args) {
			const auto pImplT = CppUtil::Basic::Ptr<ImplT>(new ImplT(args...), &HeapObj_Base::operator delete);
			static_cast<CppUtil::Basic::Ptr<HeapObj_Base>>(pImplT)->Init();
			return pImplT;
		}
		
		/*
		HeapObj 堆对象
		[使用方法]
		class ImplT final : public HeapObj<ImplT> {
		HEAPOBJ
		private: ImplT(int n);
		public: static const Ptr New(int n) { return _New(n); }
		private: virtual void Init() override;
		protected: virtual ~ImplT();};
		*/
		template<typename ImplT>
		class HeapObj : public HeapObj_Base, public std::enable_shared_from_this<ImplT> {
		public:
			typedef CppUtil::Basic::Ptr<ImplT> Ptr;
			typedef CppUtil::Basic::CPtr<ImplT> CPtr;
			typedef CppUtil::Basic::WPtr<ImplT> WPtr;
			typedef CppUtil::Basic::WCPtr<ImplT> WCPtr;

		public:
			// !!! 不可在构造函数中使用
			const Ptr This() { return shared_from_this(); }
			// !!! 不可在构造函数中使用
			const CPtr CThis() const { return shared_from_this(); }
			// !!! 不可在构造函数中使用
			const WPtr WThis() noexcept { return weak_from_this(); }
			// !!! 不可在构造函数中使用
			const WCPtr WCThis() const noexcept { return weak_from_this(); }

		protected:
			// 这里 protected 构造函数和析构函数
			// 使得用户不能直接在栈上创建 HeapObj 对象
			// 但是还是可以在栈上创建 子类
			// 所以子类也要 protected 构造函数和析构函数

			HeapObj() = default;
			virtual ~HeapObj() = default;

		private:
			using std::enable_shared_from_this<ImplT>::shared_from_this;
			using std::enable_shared_from_this<ImplT>::weak_from_this;
		};
	}
}

#endif // !_BASIC_HEAP_OBJ_HEAP_OBJ_H_
