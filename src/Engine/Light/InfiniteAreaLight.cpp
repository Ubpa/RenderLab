#include <Engine/InfiniteAreaLight.h>

#include <Basic/Image.h>
#include <Basic/BasicSampler.h>
#include <Engine/Sphere.h>
#include <Basic/Math.h>

using namespace Ubpa;

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
		double sinTheta = sin(PI<float> * (y + 0.5) / static_cast<double>(h));
		for (int x = 0; x < w; x++) {
			int idx = img->xy2idx(x, y);
			distribution[idx] = sinTheta * img->GetPixel(x, y).to_rgb().illumination();
			sum += distribution[idx];
		}
	}

	for (auto & p : distribution)
		p /= sum;

	aliasMethod.Init(distribution);
}

const rgbf InfiniteAreaLight::Sample_L(const pointf3 & p, normalf & wi, float & distToLight, float & PD) const {
	distToLight = FLT_MAX;

	if (!img || !img->IsValid()) {
		wi = BasicSampler::UniformOnSphere(PD).cast_to<normalf>();
		return intensity * colorFactor;
	}

	const auto w = img->GetWidth();
	const auto h = img->GetHeight();

	double pOfPixel;
	int idx = aliasMethod.Sample(pOfPixel); // x + y * w, 0 <= x < w
	auto xy = img->idx2xy(idx);

	float u = (xy[0] + Math::Rand_F()) / w;
	float v = (xy[1] + Math::Rand_F()) / h;

	auto sphereCoord = Sphere::SphereCoord({ u,v });
	wi = sphereCoord.ToDir();

	auto p_uv = static_cast<float>(pOfPixel * w * h);
	PD = p_uv / (2.f * PI<float> * PI<float> * std::sin(sphereCoord.theta));

	return GetColor({ u,v });
}

float InfiniteAreaLight::PDF(const pointf3 & p, const normalf & wi) const {
	if (!img || !img->IsValid())
		return BasicSampler::PDofUniformOnSphere();

	// [0, 1]
	auto texcoord = Sphere::TexcoordOf(wi);
	auto sphereCoord = Sphere::SphereCoord(texcoord);

	int w = img->GetWidth();
	int h = img->GetHeight();

	auto x = Math::Clamp(static_cast<int>(texcoord[0] * w), 0, w-1);
	auto y = Math::Clamp(static_cast<int>(texcoord[1] * h), 0, h-1);

	int idx = img->xy2idx(x, y);

	auto pOfPixel = aliasMethod.P(idx);
	auto p_uv = static_cast<float>(pOfPixel * w * h);
	return p_uv / (2.f * PI<float> * PI<float> * std::sin(sphereCoord.theta));
}

const rgbf InfiniteAreaLight::Le(const Ray & ray) const {
	auto texcoord = Sphere::TexcoordOf(ray.d.cast_to<normalf>());
	return GetColor(texcoord);
}

const rgbf InfiniteAreaLight::GetColor(const pointf2 & texcoord) const {
	if (!img || !img->IsValid())
		return intensity * colorFactor;

	return intensity * colorFactor * (img->Sample(texcoord, Image::Mode::BILINEAR)).to_rgb();
}
