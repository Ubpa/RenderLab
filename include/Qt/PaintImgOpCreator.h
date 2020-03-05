#pragma once

#include <Basic/HeapObj.h>

namespace Ubpa {
	class Op;
	class Image;

	class RawAPI_OGLW;

	class PaintImgOpCreator {
	public:
		PaintImgOpCreator(RawAPI_OGLW* pOGLW);

	public:
		class PaintImgOp final : public HeapObj {
			friend class PaintImgOpCreator;

		public:
			PaintImgOp(RawAPI_OGLW* pOGLW);

		public:
			static const Ptr<PaintImgOp> New(RawAPI_OGLW* pOGLW) {
				return Ubpa::New<PaintImgOp>(pOGLW);
			}

		public:
			bool SetOp(int w, int h);
			RawAPI_OGLW* GetOGLW() { return pOGLW; }
			Ptr<Image> GetImg() { return img; }

		private:
			RawAPI_OGLW* pOGLW;

			Ptr<Op> initOp;
			Ptr<Op> paintOp;
			Ptr<Op> resizeOp;
			Ptr<Image> img;

		private:
			virtual ~PaintImgOp() = default;
		};

		Ptr<PaintImgOp> GenScenePaintOp();

	private:
		RawAPI_OGLW* pOGLW;
	};
}
