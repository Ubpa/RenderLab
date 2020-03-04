#include <Basic/ImgPixelSet.h>

using namespace CppUtil::Basic;
using namespace std;

ImgPixelSet::ImgPixelSet() {

}

ImgPixelSet::ImgPixelSet(size_t width, size_t height) {
	data.reserve(width * height);
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			Insert(CppUtil::Point2ui(i, j));
		}
	}
}

vector<CppUtil::Point2ui> ImgPixelSet::RandPick(size_t n) {
	vector<CppUtil::Point2ui> pixels;

	const size_t num = min(Size(), n);
	pixels.reserve(num);

	for (size_t i=0; i < num; i++)
		pixels.push_back(RandPick());

	return pixels;
}

std::vector<CppUtil::Point2ui> ImgPixelSet::PickAll() {
	vector<CppUtil::Point2ui> pixels;
	const size_t num = Size();

	for (size_t i = 0; i < num; i++)
		pixels.push_back(RandPick());

	return pixels;
}
