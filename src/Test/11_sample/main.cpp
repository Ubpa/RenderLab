#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Basic/Math.h>

#include <ROOT_PATH.h>

#include <3rdParty/docopt/docopt.h>

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <thread>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

static const char USAGE[] =
R"(Test_11_sample

    Usage:
      Test_11_sample [--threadNum=<threadNum>] [--base=<base>] [--samplenum=<samplenum>] [--loop=<loop>]

    Options:
      --base <base>            base [default: 0]
      --samplenum <samplenum>  sample num [default: 100]
      --loop <loop>            pixel sample loop [default: 64]
      --threadNum <threadNum>  thread num [default: 2]
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
		auto root = SObj::Load(ROOT_PATH + "data/SObjs/App/RTGIwRRF/CB_Glass.xml");
		auto camera = root->GetComponentInChildren<Camera>();
		auto transform = camera->GetSObj()->GetComponent<Transform>();

		for (int i = id; i < sampleNum; i += threadNum) {
			vec3 eye = vec3(2 * Math::Rand_F() - 1, Math::Rand_F()*1.5, 2 * Math::Rand_F() - 1);
			vec3 center = vec3(2 * Math::Rand_F() - 1, Math::Rand_F()*1.5, 2 * Math::Rand_F() - 1);
			transform->LookAt(eye, center);

			string sobjPath = "data/SObjs/App/RTGIwRRF/CB_Glass_tmp" + to_string(id) + ".xml";
			root->Save(ROOT_PATH + sobjPath);

			string cmd = GenCmd(loop, base + i, sobjPath);
			printf("%s\n", cmd.c_str());

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
	cmd << "--csv data/SObjs/App/RTGIwRRF/datas/data" << csvID << ".csv";

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
