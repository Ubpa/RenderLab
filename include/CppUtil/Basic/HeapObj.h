#ifndef _BASIC_HEAP_OBJ_HEAP_OBJ_H_
#define _BASIC_HEAP_OBJ_HEAP_OBJ_H_

#include <CppUtil/Basic/Ptr.h>

#define HEAP_OBJ_SETUP(CLASS) \
public:\
	typedef CppUtil::Basic::Ptr<CLASS> Ptr;\
	typedef CppUtil::Basic::WPtr<CLASS> WPtr;\
	typedef CppUtil::Basic::CPtr<CLASS> CPtr;\
protected:\
	virtual ~CLASS() = default;\
private:\
	Ptr This(){ return std::dynamic_pointer_cast<CLASS>(HeapObj::This()); }\
	CPtr CThis() const { return std::dynamic_pointer_cast<const CLASS>(HeapObj::CThis()); }

#define HEAP_OBJ_SETUP_SELF_DEL(CLASS) \
public:\
	typedef CppUtil::Basic::Ptr<CLASS> Ptr;\
	typedef CppUtil::Basic::WPtr<CLASS> WPtr;\
	typedef CppUtil::Basic::CPtr<CLASS> CPtr;\
private:\
	Ptr This(){ return std::dynamic_pointer_cast<CLASS>(HeapObj::This()); }\
	CPtr CThis() const { return std::dynamic_pointer_cast<const CLASS>(HeapObj::CThis()); }

namespace CppUtil {
	namespace Basic {
		template <typename T>
		Ptr<T> ToPtr(T * ptr) {
			return Ptr<T>(ptr, T::ProtectedDelete);
		}

		template <typename T>
		CPtr<T> ToCPtr(const T * ptr) {
			return CPtr<T>(ptr, T::ProtectedDelete);
		}

		class HeapObj : public std::enable_shared_from_this<HeapObj>{
		public:
			template <typename T>
			friend Ptr<T> ToPtr(T * ptr);
			template <typename T>
			friend CPtr<T> ToCPtr(const T * ptr);

			typedef CppUtil::Basic::Ptr<HeapObj> Ptr;
			typedef CppUtil::Basic::WPtr<HeapObj> WPtr;
			typedef CppUtil::Basic::CPtr<HeapObj> CPtr;
			Ptr This() { return shared_from_this(); }
			CPtr CThis() const { return shared_from_this(); }

		protected:
			virtual ~HeapObj() = default;
			static void ProtectedDelete(const HeapObj * ptr) { delete ptr; }

		private:
			// 在调用该函数之前必须要已存在一个 shared_ptr
			using std::enable_shared_from_this<HeapObj>::shared_from_this;
			using std::enable_shared_from_this<HeapObj>::weak_from_this;
		};
	}
}

#endif // !_BASIC_HEAP_OBJ_HEAP_OBJ_H_
