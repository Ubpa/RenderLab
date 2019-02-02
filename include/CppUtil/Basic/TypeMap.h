#ifndef _TYPE_MAP_H_
#define _TYPE_MAP_H_

#include <typeinfo>
#include <unordered_map>

namespace CppUtil {
	namespace Basic {
		using TypeInfoRef = std::reference_wrapper<const std::type_info>;

		struct Hasher {
			std::size_t operator()(TypeInfoRef code) const
			{
				return code.get().hash_code();
			}
		};

		struct EqualTo {
			bool operator()(TypeInfoRef lhs, TypeInfoRef rhs) const
			{
				return lhs.get() == rhs.get();
			}
		};

		template<typename T>
		using TypeMap = std::unordered_map<TypeInfoRef, T, Hasher, EqualTo>;
	}
}

#endif//!_TYPE_MAP_H_
