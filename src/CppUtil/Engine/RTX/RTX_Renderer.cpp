#include <CppUtil/Engine/RTX_Renderer.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/RayTracer.h>
#include <CppUtil/Engine/Camera.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>

#include <CppUtil/Basic/Image.h>
#include <CppUtil/Basic/ImgPixelSet.h>
#include <CppUtil/Basic/Math.h>

#include <thread>

#include <omp.h>

#ifdef NDEBUG
#define THREAD_NUM omp_get_num_procs() - 1
#else
#define THREAD_NUM 1
#endif //  NDEBUG

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

RTX_Renderer::RTX_Renderer(const function<RayTracer::Ptr()> & generator)
	:
	generator(generator),
	state(RendererState::Stop),
	maxLoop(200),
	curLoop(0),
	threadNum(THREAD_NUM)
{
	for (int i = 0; i < threadNum; i++) {
		auto rayTracer = generator();
		rayTracers.push_back(rayTracer);
	}
}

void RTX_Renderer::Run(Scene::Ptr scene, Image::Ptr img) {
	state = RendererState::Running;

	const float lightNum = static_cast<float>(scene->GetLights().size());

	// init rst image
	int w = img->GetWidth();
	int h = img->GetHeight();

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++)
			img->SetPixel(i, j, vec3(0));
	}
	
	// init ray tracer
	for (auto rayTracer : rayTracers)
		rayTracer->Init(scene);

	// init camera
	auto camera = scene->GetCamera();
	if (camera == nullptr) {
		// curLoop = maxLoop;
		state = RendererState::Stop;
		return;
	}
	camera->SetAspectRatio(w, h);
	camera->InitCoordinate();

	// init float image
	int imgSize = w * h;
	std::vector<std::vector<vec3>> fimg;
	for (int i = 0; i < w; i++) {
		fimg.push_back(std::vector<vec3>());
		for (int j = 0; j < w; j++)
			fimg[i].push_back(vec3(0));
	}

	// jobs
	ImgPixelSet pixSet(w, h);
	const int pixelsNum = w * h / threadNum;
	vector<vector<uvec2>> pixelSets;
	for (int i = 0; i < threadNum - 1; i++)
		pixelSets.push_back(pixSet.RandPick(pixelsNum));

	pixelSets.push_back(pixSet.PickAll());

	// rays
	vector<Ray::Ptr> rays;
	for (int i = 0; i < threadNum; i++)
		rays.push_back(ToPtr(new Ray));

	auto renderPartImg = [&](int id) {
		auto & ray = rays[id];
		auto & rayTracer = rayTracers[id];
		auto const & pixelSet = pixelSets[id];

		for (auto const & pixel : pixelSet) {
			int x = pixel.x;
			int y = pixel.y;

			float u = (x + Math::Rand_F()) / (float)w;
			float v = (y + Math::Rand_F()) / (float)h;

			camera->SetRay(ray, u, v);
			vec3 rst = rayTracer->Trace(ray);

			// 这一步可以极大的减少白噪点（特别是由点光源产生）
			float illum = Math::Illum(rst);
			if (illum > lightNum)
				rst *= lightNum / illum;

			fimg[x][y] += rst;

			img->SetPixel(x, y, sqrt(fimg[x][y] / float(curLoop + 1)));

			if (state._value == RendererState::Stop )
				return;
		}
	};

	for (curLoop = 0; curLoop < maxLoop; ++curLoop) {
		// init all workers first
		vector<thread> workers;
		for (int i = 0; i < threadNum; i++)
			workers.push_back(thread(renderPartImg, i));

		// let workers to work
		for (auto & worker : workers)
			worker.join();
	}

	state = RendererState::Stop;
}

void RTX_Renderer::Stop() {
	state = RendererState::Stop;
}

float RTX_Renderer::ProgressRate() {
	return clamp((float(curLoop) + 0.5f) / float(maxLoop), 0.f, 1.f);
}
