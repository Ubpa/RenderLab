#ifndef _BASIC_HEAP_OBJ_HEAP_OBJ_H_
#define _BASIC_HEAP_OBJ_HEAP_OBJ_H_

#include <CppUtil/Basic/Ptr.h>

namespace CppUtil {
	namespace Basic {
		// 由于 HeapObj 需要模板
		// 可能需要一个公有的基类，所以提供了一个 HeapObjBase
		class HeapObjBase {
		// 将 new 和 Delete 的权限交给了 New 函数
		template<typename ImplT, typename ...Args>
		friend const Ptr<ImplT> New(Args && ... args);

		protected:
			// 由于构造函数中不可使用 This(), WPtr()
			// 所以专门提供了一个 Init 函数
			// 该函数会在 new 对象，生成 shared_ptr 后调用
			// 所以该函数内部可使用 This(), WPtr()
			virtual void Init() {}

		protected:
			// 这里 protected 构造函数和析构函数
			// 使得用户不能直接在栈上创建 HeapObjBase 对象
			// 但是还是可以在栈上创建 子类
			// 所以子类也要 protected 构造函数和析构函数

			HeapObjBase() = default;
			virtual ~HeapObjBase() = default;

		private:
			// 供给 New 来删除
			static void Delete(HeapObjBase * obj) {
				delete obj;
			}

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
		const Ptr<ImplT> New(Args && ... args) {
			const auto pImplT = Ptr<ImplT>(new ImplT(args...), HeapObjBase::Delete);
			static_cast<Ptr<HeapObjBase>>(pImplT)->Init();
			return pImplT;
		}
		
		/*
		堆对象
		ImplT 是最终要实现的类
		BaseT 要求是 HeapObjBase 的子类
		*/
		template<typename ImplT, typename BaseT = HeapObjBase>
		class HeapObj : public BaseT, public std::enable_shared_from_this<ImplT> {
		public:
			using BaseT::BaseT;

		public:
			// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
			const Ptr<ImplT> This() { return shared_from_this(); }
			// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
			const CPtr<ImplT> This() const { return shared_from_this(); }
			// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
			const WPtr<ImplT> WThis() noexcept { return weak_from_this(); }
			// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
			const WCPtr<ImplT> WThis() const noexcept { return weak_from_this(); }

		public:
			template<typename T>
			static const Ptr<ImplT> PtrCast(const Ptr<T> & ptrT) {
				return std::dynamic_pointer_cast<ImplT>(ptrT);
			}

		protected:
			// 这里 protected 构造函数和析构函数
			// 使得用户不能直接在栈上创建 HeapObj 对象
			// 但是还是可以在栈上创建 子类
			// 所以子类也要 protected 构造函数和析构函数

			virtual ~HeapObj() = default;

		private:
			using std::enable_shared_from_this<ImplT>::shared_from_this;
			using std::enable_shared_from_this<ImplT>::weak_from_this;
		};
	}
}

#endif // !_BASIC_HEAP_OBJ_HEAP_OBJ_H_
