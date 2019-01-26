#ifndef _SINGLETON_H_
#define _SINGLETON_H_

namespace CppUtility {
	namespace Other {
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

#endif// !_SINGLETON_H_