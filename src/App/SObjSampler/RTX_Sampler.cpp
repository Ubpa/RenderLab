#include "RTX_Sampler.h"

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/RayTracer.h>
#include <CppUtil/Engine/CmptCamera.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/Ray.h>
#include <CppUtil/Engine/BVHAccel.h>

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

using namespace App;
using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

RTX_Sampler::RTX_Sampler(const function<Ptr<RayTracer>()> & generator, int maxLoop, int sampleNum)
	:
	generator(generator),
	threadNum(THREAD_NUM),
	maxLoop(maxLoop),
	sampleNum(sampleNum)
{
	for (int i = 0; i < threadNum; i++) {
		auto rayTracer = generator();
		rayTracers.push_back(rayTracer);
	}
}

void RTX_Sampler::Run(Ptr<Scene> scene, Ptr<Image> img) {
	const float lightNum = static_cast<float>(scene->GetCmptLights().size());

	jobs.clear();

	// init rst image
	int w = img->GetWidth();
	int h = img->GetHeight();

	img->Clear();

	// init ray 
	auto bvhAccel = BVHAccel::New();
	bvhAccel->Init(scene->GetRoot());
	for (auto rayTracer : rayTracers)
		rayTracer->Init(scene, bvhAccel);

	// init camera
	auto camera = scene->GetCmptCamera();
	if (camera == nullptr) {
		printf("ERROR: no camera\n");
		return;
	}
	camera->SetAspectRatioWH(w, h);
	camera->InitCoordinate();

	// jobs
	vector<vector<RGBf>> fimg(w, vector<RGBf>(h, RGBf(0)));

	ImgPixelSet pixelsSet(w, h);
	for (int i = 0; i < threadNum; i++)
		jobs.push_back(pixelsSet.RandPick(sampleNum / threadNum));

	auto renderPartImg = [&](int id) {
		auto & rayTracer = rayTracers[id];
		auto & job = jobs[id];

		for (int i = 0; i < job.size(); i++) {
			int x = job[i].x;
			int y = job[i].y;

			for (int k = 0; k < maxLoop; ++k) {
				float u = (x + Math::Rand_F()) / (float)w;
				float v = (y + Math::Rand_F()) / (float)h;

				auto ray = camera->GenRay(u, v);
				RGBf rst = rayTracer->Trace(ray);

				// 丢弃不合法的结果
				if (rst.HasNaN()) {
					k--;
					continue;
				}

				// 这一步可以极大的减少白噪点（特别是由点光源产生）
				float illum = rst.Illumination();
				if (illum > lightNum)
					rst *= lightNum / illum;

				fimg[x][y] += rst;
			}

			img->SetPixel(x, y, fimg[x][y] / float(maxLoop));
		}
	};

	// init all workers first
	vector<thread> workers;
	for (int i = 0; i < threadNum; i++)
		workers.push_back(thread(renderPartImg, i));

	// let workers to work
	for (auto & worker : workers)
		worker.join();
}
