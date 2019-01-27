#ifndef _OPERATION_H_
#define _OPERATION_H_

#include <string>
#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Operation : public HeapObj {
			HEAP_OBJ_SETUP(Operation)
		public:
			Operation(bool isHold = true);

			bool IsHold() const;
			void SetIsHold(bool isHold);
			void operator()();
			//------------
			virtual void Run() = 0;
		private:
			Operation(const Operation&) = delete;
			Operation& operator=(const Operation&) = delete;

			bool isHold;
		};
	}
}

#endif//! _FILE_H_