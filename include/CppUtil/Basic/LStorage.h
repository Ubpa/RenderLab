#ifndef _LSTORAGE_H_
#define _LSTORAGE_H_

#include <map>
#include <string>

namespace CppUtil {
	namespace Basic {
		// Local Storage
		template<typename T>
		class LStorage {
		public:
			// 第一次注册返回 true, 后续返回 false
			bool Register(const std::string & uniqueID, const T & item);

			bool Unregister(const std::string & uniqueID);

			T * GetPtr(const std::string & uniqueID);

			const T * GetPtr(const std::string & uniqueID) const;

			bool Register(const std::string & uniqueID);
		private:

			std::map<std::string, T> directory;
		};

		//----------------------------------------------------------------------

		template<typename T>
		bool LStorage<T>::Register(const std::string & uniqueID, const T & item) {
			auto target = directory.find(uniqueID);
			if (target != directory.end()) {
				target->second = item;
				return false;
			}

			directory[uniqueID] = item;
			return true;
		}

		template<typename T>
		bool LStorage<T>::Unregister(const std::string & uniqueID) {
			auto target = directory.find(uniqueID);
			if (target == directory.end())
				return false;

			directory.erase(target);
			return true;
		}

		template<typename T>
		T * LStorage<T>::GetPtr(const std::string & uniqueID) {
			auto target = directory.find(uniqueID);
			if (target == directory.end())
				return NULL;

			return &(target->second);
		}

		template<typename T>
		const T * LStorage<T>::GetPtr(const std::string & uniqueID) const {
			auto target = directory.find(uniqueID);
			if (target == directory.end())
				return NULL;

			return &(target->second);
		}

		template<typename T>
		bool LStorage<T>::Register(const std::string & uniqueID) {
			auto target = directory.find(uniqueID);
			if (target != directory.end())
				return false;

			directory[uniqueID] = new decltype(*T);
			return true;
		}
	}
}

#endif // !_LSTORAGE_H_
