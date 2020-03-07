#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	class RawAPI_OGLW;
	class Camera;

	class Roamer final : public HeapObj {
	public:
		Roamer(RawAPI_OGLW* pOGLW);

	public:
		static const Ptr<Roamer> New(RawAPI_OGLW* pOGLW) {
			return Ubpa::New<Roamer>(pOGLW);
		}

	protected:
		virtual ~Roamer() = default;

	public:
		void Init();
		void SetWH(int w, int h);

		RawAPI_OGLW* GetOGLW() { return pOGLW; }
		Ptr<Camera> GetCamera() { return camera; }

	public:
		void SetLock(bool isLock) { lock = isLock; }

	private:
		void ListenerInit();
		void UpdateCamera();

		RawAPI_OGLW* pOGLW;
		unsigned int cameraUBO;
		Ptr<Camera> camera;
		bool lock;
	};
}
