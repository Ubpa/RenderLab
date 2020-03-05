#pragma once

namespace Ubpa {
	template<typename T>
	class Singleton
	{
	public:
		static T* GetInstance() {
			static T* instance = new T;
			return instance;
		}
	protected:

		Singleton() = default;
		virtual ~Singleton() = default;
	};
}
