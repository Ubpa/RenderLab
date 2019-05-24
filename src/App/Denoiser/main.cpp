#include <CppUtil/Engine/OptixAIDenoiser.h>
#include <CppUtil/Basic/Image.h>

#include <3rdParty/docopt/docopt.h>

#include <CppUtil/Basic/StrAPI.h>

#include <ROOT_PATH.h>

#include <iostream>

using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

static const char USAGE[] =
R"(Denoiser

    Usage:
      Denoiser [--notrootpath] --imgpath=<imgPath> [--outpath==<outPath>]

    Options:
      --notrootpath            path is not from root path
      --imgpath <imgPath>      sobj path
      --outpath <outPath>      output file path
)";

void ShowArgRst(const map<string, docopt::value> & rst);

int main(int argc, char *argv[]) {
	vector<string> args{ argv + 1, argv + argc };
	auto result = docopt::docopt(USAGE, args);
	ShowArgRst(result);

	bool isNotFromRootPath = result["--notrootpath"].asBool();
	string imgpath = result["--imgpath"].asString();
	string prefix = isNotFromRootPath ? "" : ROOT_PATH;
	string outpath;
	if (result["--outpath"])
		outpath = result["--outpath"].asString();
	else {
		auto prefix = StrAPI::DelTailAfter(imgpath, '.');
		outpath = prefix + "_denoised.png";
	}

	auto img = Image::New(prefix + imgpath);
	
	OptixAIDenoiser::GetInstance().Denoise(img);
	
	img->SaveAsPNG(prefix + outpath);
	return 0;
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
