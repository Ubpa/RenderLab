#include <3rdParty/rapidjson/document.h>
#include <CppUtil/Basic/File.h>
#include <ROOT_PATH.h>

#include <iostream>

using namespace CppUtil::Basic;

void Print(rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end, int indentNum);
void PrintIndent(std::string indent, int indentNum);

int main(int argc, const char** argv)
{
	File jsonFile((ROOT_PATH + "data/out/3_KDTree.json"), File::READ);
	std::string json = jsonFile.ReadAll();
	printf("%s\n\n-----\n\n", json.c_str());
	rapidjson::Document document;
	document.Parse(json.c_str());

	Print(document.MemberBegin(), document.MemberEnd(), 0);

	return 0;
}

void Print(rapidjson::Value::ConstMemberIterator begin, rapidjson::Value::ConstMemberIterator end, int indentNum) {
	printf("{\n");
	indentNum++;
	for (auto it = begin; it != end; ++it) {
		PrintIndent("    ", indentNum);
		printf("\"%s\": ", it->name.GetString());
		if (it->value.IsInt())
			printf("%d\n", it->value.GetInt());
		else if (it->value.IsFloat())
			printf("%.17f\n", it->value.GetFloat());
		else if (it->value.IsObject()) {
			Print(it->value.MemberBegin(), it->value.MemberEnd(), indentNum);
		}
		else {
			printf("not support\n");
		}
	}
	indentNum--;
	PrintIndent("    ", indentNum);
	printf("}\n");
}

void PrintIndent(std::string indent, int indentNum) {
	while (indentNum-- > 0)
		printf("%s", indent.c_str());
}
