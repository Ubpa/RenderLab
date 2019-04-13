#ifndef _ENGINE_RTX_RTX_RENDERER_H_
#define _ENGINE_RTX_RTX_RENDERER_H_

#include <CppUtil/Basic/HeapObj.h>

#include <3rdParty/enum.h>

#include <functional>
#include <vector>
#include <mutex>

namespace CppUtil {
	namespace Basic {
		class Image;
	}

	namespace Engine {
		class Scene;

		class RayTracer;

		BETTER_ENUM(RendererState, int, Running, Stop)

		class RTX_Renderer : public Basic::HeapObj {
		public:
			RTX_Renderer(const std::function<Basic::Ptr<RayTracer>()> & generator);
			
		public:
			static const Basic::Ptr<RTX_Renderer> New(const std::function<Basic::Ptr<RayTracer>()> & generator) {
				return Basic::New<RTX_Renderer>(generator);
			}

		public:
			void Run(Basic::Ptr<Scene> scene, Basic::Ptr<Basic::Image> img);
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
			std::function<Basic::Ptr<RayTracer>()> generator;

			RendererState state;

			const int threadNum;

			TileTask tileTask;
		};
	}
}

#endif//!_ENGINE_RTX_RTX_RENDERER_H_
