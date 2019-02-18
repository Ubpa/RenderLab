#include <CppUtil/Engine/OptixAIDenoiser.h>
#include <CppUtil/Basic/Image.h>
#include <ROOT_PATH.h>

using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

int main() {
	auto img = ToPtr(new Image((ROOT_PATH + "/data/out/img.png").c_str()));
	
	OptixAIDenoiser::GetInstance().Denoise(img);
	
	img->SaveAsPNG(ROOT_PATH + "/data/out/imgDenoised.png");
	return 0;
}
