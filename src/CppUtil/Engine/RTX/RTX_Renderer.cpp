#include <CppUtil/Engine/RTX_Renderer.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>
#include <CppUtil/Engine/RayTracer.h>
#include <CppUtil/Engine/CmptCamera.h>
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

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
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
	curLoop = 0;

	const float lightNum = static_cast<float>(scene->GetCmptLights().size());

	// init rst image
	int w = img->GetWidth();
	int h = img->GetHeight();

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++)
			img->SetPixel(i, j, RGBf(0.f));
	}
	
	// init ray tracer
	for (auto rayTracer : rayTracers)
		rayTracer->Init(scene);

	// init camera
	auto camera = scene->GetCmptCamera();
	if (camera == nullptr) {
		// curLoop = maxLoop;
		state = RendererState::Stop;
		printf("ERROR: no camera\n");
		return;
	}
	camera->SetAspectRatio(w, h);
	camera->InitCoordinate();

	// jobs
	const int tileSize = 32;
	const int rowTiles = w / tileSize;
	const int tileNum = w * h / (tileSize*tileSize);

	// init float image
	int imgSize = w * h;
	vector<vector<RGBf>> imgTiles(tileNum, vector<RGBf>(tileSize*tileSize, RGBf(0.f)));

	// rays
	vector<Engine::Ray> rays(threadNum);

	auto renderPartImg = [&](int id) {
		auto & ray = rays[id];
		auto & rayTracer = rayTracers[id];

		int tileID = id;
		int tileRow = tileID / rowTiles;
		int tileCol = tileID - tileRow * rowTiles;
		int baseX = tileCol * tileSize;
		int baseY = tileRow * tileSize;
		int idxInTile = 0;
		int xInTile = 0;
		int yInTile = 0;

		while (tileID < tileNum) {
			int x = baseX + xInTile;
			int y = baseY + yInTile;

			float u = (x + Math::Rand_F()) / (float)w;
			float v = (y + Math::Rand_F()) / (float)h;

			camera->SetRay(ray, u, v);
			RGBf rst = rayTracer->Trace(ray);

			// 这一步可以极大的减少白噪点（特别是由点光源产生）
			float illum = rst.Illumination();
			if (illum > lightNum)
				rst *= lightNum / illum;

			imgTiles[tileID][idxInTile] += rst;

			if (state._value == RendererState::Stop)
				return;

			xInTile++;
			idxInTile++;
			if (xInTile == tileSize) {
				xInTile = 0;
				yInTile++;
				if (yInTile == tileSize) {
					for (int j = 0; j < tileSize; j++) {
						for (int i = 0; i < tileSize; i++) {
							img->SetPixel(i + baseX, j + baseY,
								imgTiles[tileID][j*tileSize + i] / float(curLoop + 1));
						}
					}

					tileID += threadNum;
					tileRow = tileID / rowTiles;
					tileCol = tileID - tileRow * rowTiles;
					baseX = tileCol * tileSize;
					baseY = tileRow * tileSize;
					yInTile = 0;
					idxInTile = 0;
				}
			}
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

		if (state._value == RendererState::Stop)
			break;
	}

	state = RendererState::Stop;
}

void RTX_Renderer::Stop() {
	state = RendererState::Stop;
}

float RTX_Renderer::ProgressRate() {
	return Math::Clamp((float(curLoop) + 0.5f) / float(maxLoop), 0.f, 1.f);
}
