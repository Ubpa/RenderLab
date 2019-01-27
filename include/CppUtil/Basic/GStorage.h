#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <CppUtil/Basic/Singleton.h>
#include <CppUtil/Basic/LStorage.h>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class GStorage final : public Singleton<GStorage<T>>, public LStorage<T> {
			friend class Singleton<GStorage<T>>;
		public:
			static GStorage<T> * GetInstance() {
				return Singleton<GStorage<T>>::GetInstance();
			}
		private:
			GStorage() = default;
			~GStorage() = default;
		};
	}
}

#endif