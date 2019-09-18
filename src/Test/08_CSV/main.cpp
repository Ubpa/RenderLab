#include <CppUtil/Basic/CSVSaver.h>

#include <ROOT_PATH.h>

using namespace CppUtil::Basic;
using namespace std;

int main()
{
	CSVSaver<float> csvSaver({ "key0", "key1" });

	csvSaver.AddLine({ 1.f,2.f });
	csvSaver.AddLine({ 3.f,4.f });

	csvSaver.Save(ROOT_PATH + "data/out/test19.csv");

	return 0;
}
