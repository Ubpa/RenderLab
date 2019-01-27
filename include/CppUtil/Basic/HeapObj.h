#ifndef _HEAP_OBJ_H_
#define _HEAP_OBJ_H_

#include <CppUtil/Basic/Ptr.h>

#define HEAP_OBJ_SETUP(CLASS) \
public:\
	typedef CppUtil::Basic::Ptr<CLASS> Ptr;\
	typedef CppUtil::Basic::CPtr<CLASS> CPtr;\
	virtual char * GetClassName(){ return "#CLASS"; }\
	Ptr This(){ return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }\
	CPtr CThis() const { return std::dynamic_pointer_cast<const CLASS>(shared_from_this()); }\
protected:\
	virtual ~CLASS() = default;

namespace CppUtil {
	namespace Basic {
		template <typename T>
		Ptr<T> ToPtr(T * op) {
			return Ptr<T>(op, T::ProtectedDelete);
		}

		template <typename T>
		CPtr<T> ToCPtr(const T * op) {
			return CPtr<T>(op, T::ProtectedDelete);
		}

		class HeapObj : public std::enable_shared_from_this<HeapObj>{
		public:
			template <typename T>
			friend Ptr<T> ToPtr(T * op);
			template <typename T>
			friend CPtr<T> ToCPtr(const T * op);
		protected:
			virtual ~HeapObj() = default;
			static void ProtectedDelete(const HeapObj * op);
			using std::enable_shared_from_this<HeapObj>::shared_from_this;
		};
	}
}

#endif // !_HEAP_OBJ_H_
