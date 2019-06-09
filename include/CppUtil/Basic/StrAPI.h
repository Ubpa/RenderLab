#ifndef _CPPUTIL_BASIC_STR_API_H_
#define _CPPUTIL_BASIC_STR_API_H_

#include <string>
#include <vector>

namespace CppUtil {
	namespace Basic {
		namespace StrAPI {
			const std::string Head(const std::string & str, int n);
			const std::string Tail(const std::string & str, int n);

			bool IsBeginWith(const std::string & str, const std::string & suffix);
			bool IsEndWith(const std::string & str, const std::string & postfix);

			const std::vector<std::string> Spilt(const std::string & str, const std::string & separator);

			// rst = str[0] + separator + str[1] + separator + ... + str[strs.size()-1]
			const std::string Join(const std::vector<std::string> & strs, const std::string & separator = "");

			const std::string Replace(const std::string & str, const std::string & orig, const std::string & target);

			// inclusive
			const std::string DelTailAfter(const std::string & str, char c);
		}
	}
}

#endif // !_CPPUTIL_BASIC_STR_API_H_
