#ifndef _POOL_H_
#define _POOL_H_

#include <vector>
#include <map>

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Pool {
		public:
			static Pool * GetInstance();
			//------------
			T * Get();
			void Free(T * t);
		private:
			Pool();
			Pool(const Pool&);
			Pool& operator=(const Pool&);
			static Pool * instance;
			//------------
			std::vector<T*> ptrVec;
			std::map<T*, size_t> spare2Idx;
			std::map<T*, size_t> busy2Idx;

		};

		template<typename T>
		static Pool<T> * GetInstance() {
			return instance;
		}

		template<typename T>
		static Pool<T> * instance = Pool<T>::GetInstance();

		template<typename T>
		T * Pool<T>::Get() {
			T * rst = NULL;
			if (spare2Idx.empty()) {
				rst = new T();
				ptrVec.push_back(rst);
				busy2Idx[rst] = ptrVec.size() - 1;
			}
			else {
				rst = spare2Idx.begin()->first;
				busy2Idx[rst] = spare2Idx[rst];
				spare2Idx.erase(spare2Idx.begin());
			}
			return rst;
		}

		template<typename T>
		void Pool<T>::Free(T * t) {
			spare2Idx[t] = busy2Idx[t];
			busy2Idx.erase(t);
		}
	}
}

#endif // !_POOL_H_