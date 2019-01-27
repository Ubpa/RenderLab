#include "GenData_TV.h"

#include <CppUtil/RTX/ConstTexture.h>
#include <CppUtil/RTX/ImgTexture.h>
#include <CppUtil/RTX/Skybox.h>
#include <CppUtil/Basic/Image.h>

using namespace CppUtil::Basic;
using namespace RTX;
using namespace glm;
using namespace std;

// TexT : Texture Type
const float TexT_ConstTexture = 0.0f;
const float TexT_ImgTexture   = 1.0f;
const float TexT_Skybox       = 2.0f;

GenData_TV::GenData_TV(std::vector<float> & packData)
	: packData(packData) { }

void GenData_TV::Visit(ConstTexture::CPtr constTexture) {
	if (constTexture == NULL)
		return;

	auto targetPair = tex2idx.find(constTexture);
	if (targetPair != tex2idx.end())
		return;

	tex2idx[constTexture] = texData.size();

	texData.push_back(TexT_ConstTexture);

	texData.push_back(packData.size() / 4);
	const rgb color = constTexture->GetColor();
	packData.push_back(color.r);
	packData.push_back(color.g);
	packData.push_back(color.b);
	packData.push_back(0);
}

void GenData_TV::Visit(ImgTexture::CPtr imgTexture) {
	if (imgTexture == NULL)
		return;

	auto targetTexIdx = tex2idx.find(imgTexture);
	if (targetTexIdx != tex2idx.end())
		return;

	tex2idx[imgTexture] = texData.size();

	texData.push_back(TexT_ImgTexture);

	if (imgTexture->GetImg() == NULL) {
		texData.push_back(-1);
		return;
	}

	auto targetImgIdx = img2idx.find(imgTexture->GetImg());
	if (targetImgIdx == img2idx.end()) {
		size_t idx = img2idx.size();
		img2idx[imgTexture->GetImg()] = idx;
	}

	texData.push_back(img2idx[imgTexture->GetImg()]);
}

void GenData_TV::Visit(Skybox::CPtr skybox) {
	if (skybox == NULL || !skybox->IsValid())
		return;

	auto targetTexIdx = tex2idx.find(skybox);
	if (targetTexIdx != tex2idx.end())
		return;

	tex2idx[skybox] = texData.size();

	texData.push_back(TexT_Skybox);
	
	skyboxImgs = SkyboxImgs(skybox->GetSkyboxImgs());
}