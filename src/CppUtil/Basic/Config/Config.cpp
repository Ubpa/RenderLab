#include <CppUtil/Basic/Config.h>
#include <regex>
#include <sstream>
#include <iostream>
#include <vector>

using namespace CppUtil::Basic;
using namespace std;

const string syntax =
"# [ Syntax ]\n"
"# string strT = \"(string)\";\n"
"# string intT = \"(int)\";\n"
"# string hexT = \"(hex)\";\n"
"# string floatT = \"(float)\";\n"
"#  \n"
"# string blank = \"(?:[ \\\\t]*)\";\n"
"# string vName = \"([_a-zA-Z][_0-9a-zA-Z]*)\";\n"
"# \n"
"# string strVal = \"(?:\\\"([^\\\"]*)\\\")\";\n"
"# string strExp = \"(?:\" + strT + blank + vName + blank + \"=\" + blank + strVal + blank + \";)\";\n"
"# \n"
"# string intVal = \"([1-9][0-9]*|0)\";\n"
"# string intExp = \"(?:\" + intT + blank + vName + blank + \"=\" + blank + intVal + blank + \";)\";\n"
"# \n"
"# string binVal = \"([01]{1,32})\";\n"
"# string binExp = \"(?:\" + binT + blank + vName + blank + \"=\" + blank + binVal + blank + \";)\";\n"
"# \n"
"# string hexVal = \"([0-9A-F]{1,8})\";\n"
"# string hexExp = \"(?:\" + hexT + blank + vName + blank + \"=\" + blank + hexVal + blank + \";)\";\n"
"# \n"
"# string floatVal = \"((?:[1-9][0-9]*|0)(?:\\\\.[0-9]*)?)\";\n"
"# string floatExp = \"(?:\" + floatT + blank + vName + blank + \"=\" + blank + floatVal + blank + \";)\";\n"
"# \n"
"# string allExp = \"(?:\" + strExp + \"|\" + intExp + \"|\" + floatExp + \"|\" + hexExp + \"|\" + binExp + \")\";\n"
"# string note = \"(?:#.*)\";\n"
"# string line = \"(?:\" + blank + allExp + \"?\" + blank + note + \"?\" + \"\\n?)\";\n";

Config::Config()
	:valid(false) { }

Config::Config(const std::string & fileName) {
	Load(fileName);
}

bool Config::Load(const std::string & fileName) {
	File file(fileName, File::READ);
	valid = file.IsValid();
	if (!valid) {
		printf("Config file [%s] can't open\n", fileName.c_str());
		return false;
	}
	string line;
	while (line = file.ReadLine(), line.size()>0) {
		if (!DecodeLine(line)) {
			printf("Config failed\n");
			valid = false;
			return false;
		}
	}
	return true;
}

bool Config::DecodeLine(const string & data) {
	string strT = "(string)";
	string intT = "(int)";
	string floatT = "(float)";
	string hexT = "(hex)";
	string binT = "(bin)";

	string blank = "(?:[ \\t]*)";
	string vName = "([_a-zA-Z][_0-9a-zA-Z]*)";

	string strVal = "(?:\"([^\"]*)\")";
	string strExp = "(?:" + strT + blank + vName + blank + "=" + blank + strVal + blank + ";)";

	string intVal = "([1-9][0-9]*|0)";
	string intExp = "(?:" + intT + blank + vName + blank + "=" + blank + intVal + blank + ";)";

	string binVal = "([01]{1,32})";
	string binExp = "(?:" + binT + blank + vName + blank + "=" + blank + binVal + blank + ";)";

	string hexVal = "([0-9A-F]{1,8})";
	string hexExp = "(?:" + hexT + blank + vName + blank + "=" + blank + hexVal + blank + ";)";

	string floatVal = "((?:[1-9][0-9]*|0)(?:\\.[0-9]*)?)";
	string floatExp = "(?:" + floatT + blank + vName + blank + "=" + blank + floatVal + blank + ";)";

	string allExp = "(?:" + strExp + "|" + intExp + "|" + floatExp + "|" + hexExp + "|" + binExp + ")";
	string note = "(?:#.*)";
	string line = "(?:" + blank + allExp + "?" + blank + note + "?" + "\n?)";

	regex pattern(line);

	smatch result;
	
	try {
		bool compileSuccess = regex_match(data, result, pattern);
	}
	catch (regex_error e) {
		cout << e.what() << "\ncode: " << e.code() << endl;
		return false;
	}


	if (!regex_match(data, result, pattern)) {
		printf("ERROR : The format is not correct.\nSyntax is : \n%s\n", syntax.c_str());
		return false;
	}
	
	vector<int> idxVec;
	for (size_t i = 1; i < result.size(); i++) {
		if (result[i].str().size() > 0)
			idxVec.push_back(i);
	}

	for (size_t i = 0; i < idxVec.size(); i += 3) {
		size_t idx = idxVec[i];
		string type = result[idxVec[i]].str();
		string varName = result[idxVec[i + 1]].str();
		string valueStr = result[idxVec[i + 2]].str();
		if (type == "string") {
			printf("Config: string %s = \"%s\"\n", varName.c_str(), valueStr.c_str());
			strDirectory.Register(varName, valueStr);
		}
		else if (type == "float") {
			istringstream iss(valueStr);
			float val;
			iss >> val;
			printf("Config: float %s = %f\n", varName.c_str(), val);
			floatDirectory.Register(varName, val);
		}
		else if (type == "int") {
			istringstream iss(valueStr);
			int val;
			iss >> val;
			printf("Config: int %s = %d\n", varName.c_str(), val);
			intDirectory.Register(varName, val);
		}
		else if (type == "hex") {
			unsigned int val = 0;
			for (auto it = valueStr.rbegin(); it != valueStr.rend(); it++) {
				char c = *it;
				val += '0' <= c && c <= '9' ? c - '0' : c - 'A';
				val <<= 4;
			}
			printf("Config: hex %s = %s(%u)\n", varName.c_str(), valueStr.c_str(), val);
			unsignedIntDirectory.Register(varName, val);
		}
		else if (type == "bin") {
			unsigned int val = 0;
			for (size_t i = 0; i < valueStr.size(); i++) {
				val <<= 1;
				val += valueStr[i] - '0';
			}
			printf("Config: bin %s = %s(%u)\n", varName.c_str(), valueStr.c_str(), val);
			unsignedIntDirectory.Register(varName, val);
		}
		else {
			printf("ERROR : Type [%s] is unknown\nvarName : %s\nvalue : %s\n",
				result[idxVec[i]].str().c_str(), result[i+1].str().c_str(), result[i+2].str().c_str());
			return false;
		}
	}
	return true;
}

bool Config::IsValid() const {
	return valid;
}

const string * Config::GetStrPtr(const std::string & id) const {
	return strDirectory.GetPtr(id);
}

const float * Config::GetFloatPtr(const std::string & id) const {
	return floatDirectory.GetPtr(id);
}

const int * Config::GetIntPtr(const std::string & id) const {
	return intDirectory.GetPtr(id);
}

const unsigned int * Config::GetUnsignedIntPtr(const std::string & id) const {
	return unsignedIntDirectory.GetPtr(id);
}


bool Config::GetVal(const std::string & id, float & val, float defaultVal) const {
	auto p = GetFloatPtr(id);
	if (p == NULL) {
		val = defaultVal;
		return false;
	}
	
	val = *p;
	return true;
}
bool Config::GetVal(const std::string & id, int & val, int defaultVal) const {
	auto p = GetIntPtr(id);
	if (p == NULL) {
		val = defaultVal;
		return false;
	}

	val = *p;
	return true;
}
bool Config::GetVal(const std::string & id, unsigned int & val, unsigned int defaultVal) const {
	auto p = GetUnsignedIntPtr(id);
	if (p == NULL) {
		val = defaultVal;
		return false;
	}

	val = *p;
	return true;
}
bool Config::GetVal(const std::string & id, std::string & val, const string & defaultVal) const {
	auto p = GetStrPtr(id);
	if (p == NULL) {
		val = defaultVal;
		return false;
	}

	val = *p;
	return true;
}