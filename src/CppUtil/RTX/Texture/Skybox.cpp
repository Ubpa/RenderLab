#include <CppUtil/RTX/Skybox.h>

#include <CppUtil/Basic/Image.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

Skybox::Skybox(const vector<string> & skybox) {
	if (skybox.size() != 6)
		return;

	// loads a cubemap texture from 6 individual texture faces
	// order:
	// +X (right)
	// -X (left)
	// +Y (top)
	// -Y (bottom)
	// +Z (front) 
	// -Z (back)
	// -------------------------------------------------------
	for (size_t i = 0; i < 6; i++)
	{
		auto img = new Image(skybox[i].c_str(), false);
		if (!img->IsValid()) {
			printf("ERROR: Skybox texture failed to load at path: %s\n", skybox[i].c_str());
			imgs.clear();
			return;
		}

		imgs.push_back(CppUtil::Basic::CPtr<Image>(img));
	}
}

bool Skybox::IsValid() const {
	return imgs.size() == 6;
}

rgb Skybox::Value(float u, float v, const vec3 & p) const {
	vec3 leftP(-p.x, p.y, p.z);

	size_t maxDim=0;
	for (size_t i = 1; i < 3; i++) {
		if (abs(leftP[i]) > abs(leftP[maxDim]))
			maxDim = i;
	}

	size_t uDim[3] = { 2, 0, 0 };
	size_t vDim[3] = { 1, 2, 1 };
	bool flip[3][2] = { {true,false},{false,true},{false,true} };
	bool flipDim[3] = { 0,1,0 };

	vec2 texcoords((leftP[uDim[maxDim]] / abs(leftP[maxDim]) + 1) / 2, (leftP[vDim[maxDim]] / abs(leftP[maxDim]) + 1) / 2);
	if (flip[maxDim][leftP[maxDim] <= 0])
		texcoords[flipDim[maxDim]] = 1 - texcoords[flipDim[maxDim]];
	if (maxDim != 1)
		texcoords[1] = 1 - texcoords[1];
	/*
	if (maxDim == 0) {
		texcoords.y = (leftP.y / abs(leftP.x) + 1) / 2;
		if (leftP.x > 0)
			texcoords.x = 1 - (leftP.z / abs(leftP.x) + 1) / 2; 
		else
			texcoords.x = (leftP.z / abs(leftP.x) + 1) / 2;
	}
	else if (maxDim == 1) {
		texcoords.x = (leftP.x / abs(leftP.y) + 1) / 2;
		if (leftP.y > 0)
			texcoords.y = 1 - (leftP.z / abs(leftP.y) + 1) / 2;
		else
			texcoords.y = (leftP.z / abs(leftP.y) + 1) / 2;
	}
	else {
		texcoords.y = (leftP.y / abs(leftP.z) + 1) / 2;
		if (leftP.z > 0)
			texcoords.x = (leftP.x / abs(leftP.z) + 1) / 2;
		else
			texcoords.x = 1 - (leftP.x / abs(leftP.z) + 1) / 2;
	}
	*/
	size_t imgIdx = 2 * maxDim + (leftP[maxDim] > 0 ? 0 : 1);
	size_t width = imgs[imgIdx]->GetWidth();
	size_t height = imgs[imgIdx]->GetHeight();
	auto pixel = imgs[imgIdx]->GetPixel_F(clamp<size_t>(width*texcoords[0], 0, width - 1), clamp<size_t>(height*texcoords[1], 0, height - 1));
	return rgb(pixel.r, pixel.g, pixel.b);
}