#pragma once

#include <Basic/HeapObj.h>

#include <functional>
#include <vector>
#include <mutex>

namespace Ubpa {
	class Image;
	class Scene;

	class RayTracer;
	class BVHAccel;

	enum class RendererState {
		Running, Stop
	};

	class RTX_Renderer : public HeapObj {
	public:
		RTX_Renderer(const std::function<Ptr<RayTracer>()>& generator);

	public:
		static const Ptr<RTX_Renderer> New(const std::function<Ptr<RayTracer>()>& generator) {
			return Ubpa::New<RTX_Renderer>(generator);
		}

	public:
		void Run(Ptr<Scene> scene, Ptr<Image> img);
		void Stop();
		RendererState GetState() const { return state; }
		float ProgressRate();

	public:
		volatile int maxLoop;

	private:
		class TileTask {
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
			std::mutex m;
		};

	private:
		std::function<Ptr<RayTracer>()> generator;

		RendererState state;

		const int threadNum;

		TileTask tileTask;

		Ptr<BVHAccel> bvhAccel;
	};
}
