#include <CppUtil/Engine/RTX_Renderer.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/RayTracer.h>
#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/ImgPixelSet.h>
#include <CppUtil/Basic/Math.h>

#include <omp.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

RTX_Renderer::RTX_Renderer(CppUtil::Basic::Ptr<RayTracer> rayTracer)
	: rayTracer(rayTracer), isStop(false), maxLoop(100), curLoop(0) { }

void RTX_Renderer::Run(Image::Ptr img) {
	omp_set_num_threads(omp_get_num_procs() - 1);

	int w = img->GetWidth();
	int h = img->GetHeight();

	auto scene = rayTracer->GetScene();
	auto camera = scene->GetMainCamera();
	camera->SetWidthHeight(w, h);
	auto cameraMatrix = camera->GetSObj()->GetLocalToWorldMatrix();

	int imgSize = w * h;

	for (curLoop = 0; curLoop < maxLoop; ++curLoop) {
		ImgPixelSet pixSet(w, h);
		auto randSet = pixSet.PickAll();

#pragma omp parallel for schedule(dynamic, 256)
		for (int pixelIdx = 0; pixelIdx < imgSize; pixelIdx++) {
			auto xy = randSet[pixelIdx];
			int x = xy.x;
			int y = xy.y;

			float u = (x + Math::Rand_F()) / (float)w;
			float v = (y + Math::Rand_F()) / (float)h;

			auto ray = camera->GenRay(u, v);
			ray->Transform(cameraMatrix);
			vec3 rst = rayTracer->Trace(ray);

			auto origPixel = img->GetPixel_F(x, y);
			vec3 origColor(origPixel.r*origPixel.r, origPixel.g*origPixel.g, origPixel.b*origPixel.b);
			vec3 newColor = sqrt((float(curLoop) * origColor + rst) / float(curLoop + 1));
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
	return clamp((float(curLoop) + 0.5f) / float(maxLoop), 0.f, 1.f);
}
