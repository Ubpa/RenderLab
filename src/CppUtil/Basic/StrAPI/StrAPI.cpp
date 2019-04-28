#include <CppUtil/Basic/StrAPI.h>

using namespace CppUtil::Basic;
using namespace std;


bool StrAPI::IsBeginWith(const string & str, const string & suffix) {
	if (suffix.size() > str.size())
		return false;

	for (string::size_type i = 0; i < suffix.size(); i++) {
		if (str[i] != suffix[i])
			return false;
	}

	return true;
}

bool StrAPI::IsEndWith(const string & str, const string & postfix) {
	if (postfix.size() > str.size())
		return false;

	const auto beginIdx = str.size() - postfix.size();
	for (string::size_type i = 0; i < postfix.size(); i++) {
		if (str[beginIdx + i] != postfix[i])
			return false;
	}

	return true;
}
