#ifndef _BASIC_HEADER_SINGLETON_H_
#define _BASIC_HEADER_SINGLETON_H_

namespace CppUtil {
	namespace Basic {
		template<typename T>
		class Singleton
		{
		public:
			static T * GetInstance() {
				static T * instance = new T;
				return instance;
			}
		protected:

			Singleton() = default;
			virtual ~Singleton() = default;
		};
	}
}

#endif// !_BASIC_HEADER_SINGLETON_H_