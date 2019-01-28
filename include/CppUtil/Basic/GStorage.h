#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <CppUtil/Basic/Singleton.h>
#include <CppUtil/Basic/LStorage.h>

namespace CppUtil {
	namespace Basic {
		
		class GS{
		private:
			template<typename T>
			class _GStorage final : public Singleton<_GStorage<T>>, public LStorage<T> {
				friend class Singleton<_GStorage<T>>;
			public:
				static _GStorage<T> * GetInstance() {
					return Singleton<_GStorage<T>>::GetInstance();
				}
			private:
				_GStorage() = default;
				~_GStorage() = default;
			};

		private:
			GS() = default;

		public:
			template<typename T>
			static bool Reg(const std::string & uniqueID, const T & item) {
				return _GStorage<T>::GetInstance()->Reg(uniqueID, item);
			}

			template<typename T>
			static void GetP(const std::string & uniqueID, T * & p) {
				p = _GStorage<T>::GetInstance()->GetP(uniqueID);
			}

			template<typename T>
			static bool GetV(const std::string & uniqueID, T & val) {
				auto p = _GStorage<T>::GetInstance()->GetP(uniqueID);
				if (p == nullptr)
					return false;

				val = *p;
				return true;
			}
		};
	}
}

#endif