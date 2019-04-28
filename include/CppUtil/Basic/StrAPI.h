#ifndef _CPPUTIL_BASIC_STR_API_H_
#define _CPPUTIL_BASIC_STR_API_H_

#include <string>

namespace CppUtil {
	namespace Basic {
		namespace StrAPI {
			bool IsBeginWith(const std::string & str, const std::string & suffix);
			bool IsEndWith(const std::string & str, const std::string & postfix);
		}
	}
}

#endif // !_CPPUTIL_BASIC_STR_API_H_
