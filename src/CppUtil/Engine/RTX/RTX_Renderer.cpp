#include <CppUtil/Engine/RTX_Renderer.h>

#include <CppUtil/Engine/Scene.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/ImgPixelSet.h>
#include <CppUtil/Basic/Math.h>

#include <omp.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

RTX_Renderer::RTX_Renderer(CppUtil::Basic::Ptr<RayTracer> rayTracer)
	: rayTracer(rayTracer), isStop(false), maxLoop(20) { }

void RTX_Renderer::Run(Scene::Ptr scene, Image::Ptr img) {
	omp_set_num_threads(omp_get_num_procs() - 1);

	int w = img->GetWidth();
	int h = img->GetHeight();
	int imgSize = w * h;

	for (size_t sampleNum = 0; sampleNum < maxLoop; ++sampleNum) {
		curLoop = sampleNum;

		ImgPixelSet pixSet(w, h);
		auto randSet = pixSet.PickAll();
#pragma omp parallel for schedule(dynamic, 1024)
		for (int pixelIdx = 0; pixelIdx < imgSize; pixelIdx++) {
			auto xy = randSet[pixelIdx];
			int x = xy.x;
			int y = xy.y;

			float u = (x + Math::Rand_F()) / (float)w;
			float v = (y + Math::Rand_F()) / (float)h;

			//auto ray = scene->camera->GenRay(u, v);
			//vec3 rst = rayTracer->Trace(ray);
			vec3 rst(u, v, (u + v) / 2);

			auto origPixel = img->GetPixel_F(x, y);
			vec3 origColor(origPixel.r*origPixel.r, origPixel.g*origPixel.g, origPixel.b*origPixel.b);
			vec3 newColor = sqrt((float(sampleNum) * origColor + rst) / float(sampleNum + 1));
			img->SetPixel(x, y, newColor);
		}
		if (isStop)
			return;
	}
}

void RTX_Renderer::Stop() {
	isStop = true;
}

float RTX_Renderer::ProgressRate() {
	return (float(curLoop) + 0.5f) / float(maxLoop);
}
