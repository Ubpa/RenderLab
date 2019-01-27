#include <CppUtil/Basic/ImgPixelSet.h>

using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

ImgPixelSet::ImgPixelSet() {

}

ImgPixelSet::ImgPixelSet(size_t width, size_t height) {
	data.reserve(width * height);
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			Insert(glm::vec2(i, j));
		}
	}
}

size_t ImgPixelSet::RandPick(size_t n, vector<glm::uvec2> & pixels) {
	pixels.clear();
	const size_t num = glm::min(Size(), n);
	pixels.reserve(num);
	for (size_t i=0; i < num; i++)
		pixels.push_back(RandPick());
	return num;
}

