#include <CppUtil/RTX/RTX_Renderer.h>

#include <CppUtil/RTX/RayTracer.h>
#include <CppUtil/RTX/Scene.h>
#include <CppUtil/RTX/RayCamera.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/Math.h>

#include <omp.h>

using namespace CppUtil::Basic;
using namespace RTX;
using namespace glm;

RTX_Renderer::RTX_Renderer(CppUtil::Basic::CPtr<Scene> scene, CppUtil::Basic::Ptr<CppUtil::Basic::Image> img)
	: scene(scene), img(img), rayTracer(new RayTracer(scene->obj)) { }

void RTX_Renderer::Run() {
	omp_set_num_threads(omp_get_num_procs() - 1);

	int w = img->GetWidth();
	int h = img->GetHeight();
	int imgSize = w * h;
#pragma omp parallel for schedule(dynamic, 1024)
	for (int pixelIdx = 0; pixelIdx < imgSize; pixelIdx++) {
		int x = pixelIdx % w;
		int y = pixelIdx / w;
		float u = (x + Math::Rand_F()) / (float)w;
		float v = (y + Math::Rand_F()) / (float)h;

		auto ray = scene->camera->GenRay(u, v);
		vec3 rst = rayTracer->Trace(ray);
		img->SetPixel(x, y, rst);
	}
}
