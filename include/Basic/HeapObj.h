#pragma once

#include <Basic/Ptr.h>

namespace Ubpa {
	class HeapObj : public std::enable_shared_from_this<HeapObj> {
		// 将 new 和 Delete 的权限交给了 New 函数
		template<typename ImplT, typename ...Args>
		friend const Ptr<ImplT> New(Args&& ... args);

	protected:
		// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
		template<typename T = HeapObj>
		const Ptr<T> This() { return CastTo<T>(shared_from_this()); }
		// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
		template<typename T = HeapObj>
		const PtrC<T> This() const { return CastTo<const T>(shared_from_this()); }
		// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
		template<typename T = HeapObj>
		const WPtr<T> WThis() noexcept { return CastTo<T>(weak_from_this()); }
		// !!! 不可在构造函数中使用，将初始化任务放到 Init() 中
		template<typename T = HeapObj>
		const WPtrC<T> WThis() const noexcept { return CastTo<const T>(weak_from_this()); }

	protected:
		// 由于构造函数中不可使用 This(), WPtr()
		// 所以专门提供了一个 Init_AfterGenPtr 函数
		// 该函数会在 new 对象，生成 shared_ptr 后调用
		// 所以该函数内部可使用 This(), WPtr()
		virtual void Init_AfterGenPtr() {}

	protected:
		// 这里 protected 构造函数和析构函数
		// 使得用户不能直接在栈上创建 HeapObj 对象
		// 但是还是可以在栈上创建 子类
		// 所以子类也要 protected 构造函数和析构函数

		HeapObj() = default;
		virtual ~HeapObj() = default;

	private:
		// 供给 New 来删除
		static void Delete(HeapObj* obj) {
			delete obj;
		}

	private:
		// private new 和 delete
		// 这样用户就无法使用 new 了
		// 而且子类也没法 new

		void* operator new(size_t size) {
			if (void* mem = malloc(size))
				return mem;
			else
				throw std::bad_alloc();
		}

		void operator delete(void* mem) noexcept {
			free(mem);
		}

	private:
		using std::enable_shared_from_this<HeapObj>::shared_from_this;
		using std::enable_shared_from_this<HeapObj>::weak_from_this;
	};

	// 调用 ImplT 的构造函数，然后生成 shared_ptr，然后调用 virtual 的 Init_AfterGenPtr 函数
	template<typename ImplT, typename ...Args>
	const Ptr<ImplT> New(Args&& ... args) {
		const auto pImplT = Ptr<ImplT>(new ImplT(std::forward<Args>(args)...), HeapObj::Delete);
		auto pHeapObj = static_cast<Ptr<HeapObj>>(pImplT);
		pHeapObj->Init_AfterGenPtr();
		return pImplT;
	}
}
