#include <CppUtil/Basic/CSV.h>

#include <ROOT_PATH.h>

using namespace CppUtil::Basic;
using namespace std;

int main()
{
	CSV<float> csv({ "key0", "key1" });

	csv.AddLine({ 1.f,2.f });
	csv.AddLine({ 3.f,4.f });

	csv.Save(ROOT_PATH + "data/out/test19.csv");

	return 0;
}
