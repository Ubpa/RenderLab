#pragma once

#include <Basic/LStorage.h>
#include <string>

namespace Ubpa {
	class GS {
	private:
		template<typename ID_Type, typename T>
		class _GStorage final : public LStorage<ID_Type, T> {
		public:
			static _GStorage<ID_Type, T>* GetInstance() {
				static _GStorage instance;
				return &instance;
			}
		private:
			_GStorage() = default;
			~_GStorage() = default;
		};

	private:
		GS() = default;

		template<typename ID_Type, typename itemT, bool idToStr, bool itemToStr>
		struct Wrapper;

		template<typename ID_Type, typename itemT>
		struct Wrapper<ID_Type, itemT, false, false> {
			static bool Reg(const ID_Type& uniqueID, const itemT& item) {
				return _GStorage<ID_Type, itemT>::GetInstance()->Reg(uniqueID, item);
			}
			static itemT* GetP(const ID_Type& uniqueID) {
				return _GStorage<ID_Type, itemT>::GetInstance()->GetP(uniqueID);
			}
			static bool UnReg(const ID_Type& uniqueID) {
				return _GStorage<ID_Type, itemT>::GetInstance()->UnReg(uniqueID);
			}
		};

		template<typename ID_Type, typename itemT>
		struct Wrapper<ID_Type, itemT, true, false> {
			static bool Reg(const std::string& uniqueID, const itemT& item) {
				return _GStorage<std::string, itemT>::GetInstance()->Reg(uniqueID, item);
			}
			static itemT* GetP(const std::string& uniqueID) {
				return _GStorage<std::string, itemT>::GetInstance()->GetP(uniqueID);
			}
			static bool UnReg(const std::string& uniqueID) {
				return _GStorage<std::string, itemT>::GetInstance()->UnReg(uniqueID);
			}
		};

		template<typename ID_Type, typename itemT>
		struct Wrapper<ID_Type, itemT, true, true> {
			static bool Reg(const std::string& uniqueID, const std::string& item) {
				return _GStorage<std::string, std::string>::GetInstance()->Reg(uniqueID, item);
			}
			static itemT* GetP(const std::string& uniqueID) {
				return _GStorage<std::string, std::string>::GetInstance()->GetP(uniqueID);
			}
			static bool UnReg(const std::string& uniqueID) {
				return _GStorage<std::string, std::string>::GetInstance()->UnReg(uniqueID);
			}
		};

		template<typename ID_Type, typename itemT>
		struct Wrapper<ID_Type, itemT, false, true> {
			static bool Reg(const ID_Type& uniqueID, const std::string& item) {
				return _GStorage<ID_Type, std::string>::GetInstance()->Reg(uniqueID, item);
			}
			static itemT* GetP(const ID_Type& uniqueID) {
				return _GStorage<ID_Type, std::string>::GetInstance()->GetP(uniqueID);
			}
			static bool UnReg(const ID_Type& uniqueID) {
				return _GStorage<ID_Type, std::string>::GetInstance()->UnReg(uniqueID);
			}
		};

		template<typename ID_Type, typename itemT>
		struct WWrapper : public Wrapper<ID_Type, itemT,
			std::is_convertible<ID_Type, std::string>::value,
			std::is_convertible<itemT, std::string>::value> { };

	public:
		template<typename ID_Type, typename T>
		static bool Reg(const ID_Type& uniqueID, const T& item) {
			return WWrapper<ID_Type, T>::Reg(uniqueID, item);
		}

		template<typename ID_Type, typename T>
		static void GetP(const ID_Type& uniqueID, T*& p) {
			p = WWrapper<ID_Type, T>::GetP(uniqueID);
		}

		template<typename ID_Type, typename T>
		static bool GetV(const ID_Type& uniqueID, T& val) {
			auto p = WWrapper<ID_Type, T>::GetP(uniqueID);
			if (p == nullptr)
				return false;

			val = *p;
			return true;
		}

		template<typename T, typename ID_Type>
		static bool UnReg(const ID_Type& uniqueID) {
			return WWrapper<ID_Type, T>::UnReg(uniqueID);
		}
	};
}
