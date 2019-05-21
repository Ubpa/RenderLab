#include <CppUtil/Basic/StrAPI.h>

#include <cassert>
#include <algorithm>

using namespace CppUtil::Basic;
using namespace std;

const string StrAPI::Head(const string & str, int n) {
	assert(n >= 0);
	return str.substr(0, std::min(static_cast<size_t>(n), str.size()));
}

const string StrAPI::Tail(const string & str, int n) {
	assert(n >= 0);
	return str.substr(str.size() - n, n);
}

bool StrAPI::IsBeginWith(const string & str, const string & suffix) {
	return Head(str, static_cast<int>(suffix.size())) == suffix;
}

bool StrAPI::IsEndWith(const string & str, const string & postfix) {
	return Tail(str, static_cast<int>(postfix.size())) == postfix;
}

const vector<string> StrAPI::Spilt(const string & str, const string & separator) {
	vector<string> rst;

	if (separator.empty())
		return rst;

	size_t beginIdx = 0;
	while(true){
		size_t targetIdx = str.find(separator, beginIdx);
		if (targetIdx == string::npos) {
			rst.push_back(str.substr(beginIdx, str.size() - beginIdx));
			break;
		}
		
		rst.push_back(str.substr(beginIdx, targetIdx - beginIdx));
		beginIdx = targetIdx + separator.size();
	}

	return rst;
}

const string StrAPI::Join(const vector<string> & strs, const string & separator) {
	string rst;

	for (size_t i = 0; i < strs.size()-1; i++) {
		rst += strs[i];
		rst += separator;
	}

	rst += strs.back();

	return rst;
}

const string StrAPI::Replace(const string & str, const string & orig, const string & target) {
	return Join(Spilt(str, orig), target);
}

const std::string StrAPI::DelTailAfter(const std::string & str, char c) {
	for (size_t i = str.size() - 1; i >= 0; i--) {
		if (str[i] == c)
			return str.substr(0, i);
	}

	return str;
}
