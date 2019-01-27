#include <CppUtil/Basic/ArgManager.h>

#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

using namespace CppUtil::Basic;
using namespace std;


ArgManager::ArgManager(int argc, char** argv) {
	string tag;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			tag = string(&(argv[i][1]));
			pairs[tag] = vector<string>();
		}
		else if (tag.size() != 0)
			pairs[tag].push_back(argv[i]);
		else
			perror("arg format error\n");
	}
}
	
vector<string> ArgManager::GetAll(const string & key) {
	return pairs[key];
}

string ArgManager::Get(const string & key, size_t idx, const string & defaultValue) {
	vector<string> value = pairs[key];
	if (idx >= value.size())
		return defaultValue;
	else
		return value[idx];
}