#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <Utility/Basic/Singleton.h>
#include <Utility/Basic/LStorage.h>

namespace CppUtility {
	namespace Other {
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