#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <CppUtil/Basic/Singleton.h>
#include <CppUtil/Basic/LStorage.h>
#include <string>

namespace CppUtil {
	namespace Basic {
		
		class GS{
		private:
			template<typename ID_Type, typename T>
			class _GStorage final : public Singleton<_GStorage<ID_Type, T>>, public LStorage<ID_Type, T> {
				friend class Singleton<_GStorage<ID_Type, T>>;
			public:
				static _GStorage<ID_Type, T> * GetInstance() {
					return Singleton<_GStorage<ID_Type, T>>::GetInstance();
				}
			private:
				_GStorage() = default;
				~_GStorage() = default;
			};

		private:
			GS() = default;

			template<typename ID_Type, typename T, bool toStr>
			struct Wrapper;
			
			template<typename ID_Type, typename T>
			struct Wrapper<ID_Type, T, false> {
				static bool Reg(const ID_Type & uniqueID, const T & item) {
					return _GStorage<ID_Type, T>::GetInstance()->Reg(uniqueID, item);
				}
				static T * GetP(const ID_Type & uniqueID) {
					return _GStorage<ID_Type, T>::GetInstance()->GetP(uniqueID);
				}
			};

			template<typename ID_Type, typename T>
			struct Wrapper<ID_Type, T, true> {
				static bool Reg(const std::string & uniqueID, const T & item) {
					return _GStorage<std::string, T>::GetInstance()->Reg(uniqueID, item);
				}
				static T * GetP(const std::string & uniqueID) {
					return _GStorage<std::string, T>::GetInstance()->GetP(uniqueID);
				}
			};

		public:
			template<typename ID_Type, typename T>
			static bool Reg(const ID_Type & uniqueID, const T & item) {
				return Wrapper<ID_Type, T, std::is_convertible<ID_Type, std::string>::value>::Reg(uniqueID, item);
			}

			template<typename ID_Type, typename T>
			static void GetP(const ID_Type & uniqueID, T * & p) {
				p = Wrapper<ID_Type, T, std::is_convertible<ID_Type, std::string>::value>::GetP(uniqueID);
			}

			template<typename ID_Type, typename T>
			static bool GetV(const ID_Type & uniqueID, T & val) {
				auto p = Wrapper<ID_Type, T, std::is_convertible<ID_Type, std::string>::value>::GetP(uniqueID);
				if (p == nullptr)
					return false;

				val = *p;
				return true;
			}
		};
	}
}

#endif