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

#include <omp.h>

#include "Film.h"
#include "FilmTile.h"
#include <CppUtil/Engine/BVHAccel.h>
#include <CppUtil/Engine/FilterMitchell.h>

#ifdef NDEBUG
#define THREAD_NUM omp_get_num_procs() - 1
#else
#define THREAD_NUM 1
#endif //  NDEBUG

using namespace CppUtil;
using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

namespace CppUtil {
	namespace Engine {
		class TileTask{
		public:
			TileTask(int tileNum, int maxLoop)
				: tileNum(tileNum), maxLoop(maxLoop), curTile(0), curLoop(0) { }

		public:
			void Init(int tileNum, int maxLoop) {
				this->tileNum = tileNum;
				this->maxLoop = maxLoop;
				curTile = 0;
				curLoop = 0;
			}

		public:
			struct Task {
				Task(bool hasTask, int tileID = -1, int curLoop = -1)
					: hasTask(hasTask), tileID(tileID), curLoop(curLoop) { }

				bool hasTask;
				int tileID;
				int curLoop;
			};
			static constexpr int ERROR = -1;
			const Task GetTask() {
				if (curLoop == maxLoop)
					return Task(false);
				
				m.lock();
				auto rst = Task(true, curTile, curLoop);
				curTile++;
				if (curTile == tileNum) {
					curTile = 0;
					curLoop += 1;
				}
				m.unlock();

				return rst;
			}

			int GetCurLoop() const {
				return curLoop;
			}

		private:
			int tileNum;
			int curTile;
			int maxLoop;
			int curLoop;
			mutex m;
		};
	}
}

RTX_Renderer::RTX_Renderer(const function<Ptr<RayTracer>()> & generator)
	:
	generator(generator),
	bvhAccel(BVHAccel::New()),
	state(RendererState::Stop),
	maxLoop(200),
	threadNum(THREAD_NUM)
{
}

void RTX_Renderer::Run(Ptr<Scene> scene, Ptr<Image> img) {
	state = RendererState::Running;

	const float lightNum = static_cast<float>(scene->GetCmptLights().size());

	// init rst image

	auto film = Film::New(img, FilterMitchell::New({ 2,2 }, 0.333f, 0.333f));
	int w = img->GetWidth();
	int h = img->GetHeight();

	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++)
			img->SetPixel(i, j, RGBf(0.f));
	}

	vector<Ptr<RayTracer>> rayTracers;

	for (int i = 0; i < threadNum; i++) {
		auto rayTracer = generator();
		rayTracers.push_back(rayTracer);
	}
	
	bvhAccel->Init(scene->GetRoot());
	// init ray tracer
	for (auto rayTracer : rayTracers)
		rayTracer->Init(scene, bvhAccel);

	// init camera
	auto camera = scene->GetCmptCamera();
	if (camera == nullptr) {
		// curLoop = maxLoop;
		state = RendererState::Stop;
		printf("ERROR: no camera\n");
		return;
	}
	camera->SetAspectRatioWH(w, h);
	camera->InitCoordinate();

	// jobs
	const int tileSize = 32;
	const int rowTiles = w / tileSize;
	const int tileNum = w * h / (tileSize*tileSize);
	tileTask.Init(tileNum, maxLoop);

	// init float image
	int imgSize = w * h;
	vector<vector<RGBf>> imgTiles(tileNum, vector<RGBf>(tileSize*tileSize, RGBf(0.f)));

	auto renderPartImg = [&](int id) {
		auto & rayTracer = rayTracers[id];

		for (auto task = tileTask.GetTask(); task.hasTask; task = tileTask.GetTask()) {
			if (state._value == RendererState::Stop)
				return;

			int tileID = task.tileID;
			int tileRow = tileID / rowTiles;
			int tileCol = tileID - tileRow * rowTiles;
			int baseX = tileCol * tileSize;
			int baseY = tileRow * tileSize;

			auto filmTile = film->GenFilmTile(Framei({ baseX, baseY }, { baseX + tileSize, baseY + tileSize }));

			for (const auto pos : filmTile->AllPos()) {

				const float u = (pos.x + Math::Rand_F()) / (float)w;
				const float v = (pos.y + Math::Rand_F()) / (float)h;

				auto ray = camera->GenRay(u, v);
				RGBf radiance = rayTracer->Trace(ray);

				// 这一步可以极大的减少白噪点（特别是由点光源产生）
				float illum = radiance.Illumination();
				if (illum > lightNum)
					radiance *= lightNum / illum;

				filmTile->AddSample(pos, radiance);
			}

			film->MergeFilmTile(filmTile);
		}
	};

	// init all workers first
	vector<thread> workers;
	for (int i = 0; i < threadNum; i++)
		workers.push_back(thread(renderPartImg, i));

	// wait workers
	for (auto & worker : workers)
		worker.join();

	state = RendererState::Stop;
}

void RTX_Renderer::Stop() {
	state = RendererState::Stop;
}

float RTX_Renderer::ProgressRate() {
	return Math::Clamp((float(tileTask.GetCurLoop()) + 0.5f) / float(maxLoop), 0.f, 1.f);
}
