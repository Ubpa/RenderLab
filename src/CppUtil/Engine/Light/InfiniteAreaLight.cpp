#include <CppUtil/Engine/InfiniteAreaLight.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/BasicSampler.h>
#include <CppUtil/Engine/Sphere.h>

using namespace CppUtil;
using namespace CppUtil::Basic;
using namespace CppUtil::Engine;
using namespace std;

void InfiniteAreaLight::SetImg(Ptr<Image> img) {
	aliasMethod.Clear();
	this->img = img;

	if (!img)
		return;

	if (!img->IsValid() || img->GetChannel() != 3) {
		printf("ERROR::InfiniteAreaLight::SetImg:\n"
			"\t""img is invalid\n");
		return;
	}

	auto w = img->GetWidth();
	auto h = img->GetHeight();
	vector<double> distribution(w*h);
	double sum = 0.f;
	for (int y = 0; y < h; y++) {
		double sinTheta = sin(Math::PI * (y + 0.5) / static_cast<double>(h));
		for (int x = 0; x < w; x++) {
			int idx = img->xy2idx(x, y);
			distribution[idx] = sinTheta * img->GetPixel(x, y).ToRGB().Illumination();
			sum += distribution[idx];
		}
	}

	for (auto & p : distribution)
		p /= sum;

	aliasMethod.Init(distribution);
}

const RGBf InfiniteAreaLight::Sample_L(const Point3 & p, Normalf & wi, float & distToLight, float & PD) const {
	distToLight = FLT_MAX;

	if (!img || !img->IsValid()) {
		wi = BasicSampler::UniformOnSphere(PD);
		return intensity * colorFactor;
	}

	const auto w = img->GetWidth();
	const auto h = img->GetHeight();

	double pOfPixel;
	int idx = aliasMethod.Sample(pOfPixel); // x + y * w, 0 <= x < w
	auto xy = img->idx2xy(idx);

	float u = (xy.x + Math::Rand_F()) / w;
	float v = (xy.y + Math::Rand_F()) / h;

	auto sphereCoord = Sphere::SphereCoord({ u,v });
	wi = sphereCoord.ToDir();

	auto p_uv = static_cast<float>(pOfPixel * w * h);
	PD = p_uv / (2.f * Math::PI * Math::PI * std::sin(sphereCoord.theta));

	return GetColor({ u,v });
}

float InfiniteAreaLight::PDF(const Point3 & p, const Normalf & wi) const {
	if (!img || !img->IsValid())
		return BasicSampler::PDofUniformOnSphere();

	// [0, 1]
	auto texcoord = Sphere::TexcoordOf(wi);
	auto sphereCoord = Sphere::SphereCoord(texcoord);

	int w = img->GetWidth();
	int h = img->GetHeight();

	auto x = Math::Clamp(static_cast<int>(texcoord.x * w), 0, w-1);
	auto y = Math::Clamp(static_cast<int>(texcoord.y * h), 0, h-1);

	int idx = img->xy2idx(x, y);

	auto pOfPixel = aliasMethod.P(idx);
	auto p_uv = static_cast<float>(pOfPixel * w * h);
	return p_uv / (2.f * Math::PI * Math::PI * std::sin(sphereCoord.theta));
}

const RGBf InfiniteAreaLight::Le(const ERay & ray) const {
	auto texcoord = Sphere::TexcoordOf(ray.d);
	return GetColor(texcoord);
}

const RGBf InfiniteAreaLight::GetColor(const Point2 & texcoord) const {
	if (!img || !img->IsValid())
		return intensity * colorFactor;

	return intensity * colorFactor * (img->Sample(texcoord, Image::Mode::BILINEAR)).ToRGB();
}
