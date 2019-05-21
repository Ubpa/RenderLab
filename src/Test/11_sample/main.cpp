#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/CmptCamera.h>
#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

#include <3rdParty/docopt/docopt.h>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <thread>

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

static const char USAGE[] =
R"(Test_11_sample

    Usage:
      Test_11_sample [--threadNum=<threadNum>] [--base=<base>] [--samplenum=<samplenum>] [--loop=<loop>]

    Options:
      --base <base>            base [default: 0]
      --samplenum <samplenum>  sample num [default: 100]
      --loop <loop>            pixel sample loop [default: 4096]
      --threadNum <threadNum>  thread num [default: 1]
)";

string GenCmd(int maxLoop, int csvID, const string & sobjPath);
void ShowArgRst(const map<string, docopt::value> & rst);

int main(int argc, const char** argv)
{
	vector<string> args{ argv + 1, argv + argc };
	auto result = docopt::docopt(USAGE, args);
	ShowArgRst(result);

	Math::RandSetSeedByCurTime();

	const int base = static_cast<int>(result["--base"].asLong());
	const int sampleNum = static_cast<int>(result["--samplenum"].asLong());
	const int loop = static_cast<int>(result["--loop"].asLong());
	const int threadNum = static_cast<int>(result["--threadNum"].asLong());

	vector<thread> workers;
	for (int k = 0; k < threadNum; k++)
		workers.push_back(thread([=](int id) {
		auto root = SObj::Load(ROOT_PATH + "data/SObjs/App/RTGIwRRF/CB_Glass.sobj");
		auto camera = root->GetComponentInChildren<CmptCamera>();
		auto transform = camera->GetSObj()->GetComponent<CmptTransform>();

		for (int i = id; i < sampleNum; i += threadNum) {
			Point3 eye = Point3(2 * Math::Rand_F() - 1, Math::Rand_F()*1.5, 2 * Math::Rand_F() - 1);

			// 靠近中心位置
			Point3 center = Point3(Math::Rand_F() - 0.5, Math::Rand_F()*1 + 0.25, Math::Rand_F() - 0.5);
			transform->LookAt(eye, center);

			string sobjPath = "data/SObjs/App/RTGIwRRF/CB_Glass_tmp" + to_string(id) + ".sobj";
			root->Save(ROOT_PATH + sobjPath);

			string cmd = GenCmd(loop, base + i, sobjPath);
			printf("%s\n\n", cmd.c_str());

			system(cmd.c_str());
		}
	}, k));

	for (auto & worker : workers)
		worker.join();

	return 0;
}

string GenCmd(int maxLoop, int csvID, const string & sobjPath) {
	stringstream cmd;
	cmd << ROOT_PATH << "bin/SObjSampler.exe ";
	cmd << "--maxloop " << maxLoop <<" ";
	cmd << "--sobj " << sobjPath << " ";
	cmd << "--csv data/SObjs/App/RTGIwRRF/datas/data" << csvID << ".csv ";
	cmd << "> nul ";

	return cmd.str();
}

void ShowArgRst(const map<string, docopt::value> & rst) {
	cout << "[ Arg Result ]" << endl << endl;
	cout << "{" << endl;
	bool first = true;
	for (auto const & arg : rst) {
		if (first)
			first = false;
		else
			cout << "," << endl;

		cout << '"' << arg.first << '"' << ": " << arg.second;
	}
	cout << endl << "}" << endl << endl;
}
