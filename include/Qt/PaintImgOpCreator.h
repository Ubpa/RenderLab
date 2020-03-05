#pragma once

#include <Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Op;
		class Image;
	}

	namespace QT {
		class RawAPI_OGLW;

		class PaintImgOpCreator {
		public:
			PaintImgOpCreator(RawAPI_OGLW * pOGLW);

		public:
			class PaintImgOp final : public Basic::HeapObj {
				friend class PaintImgOpCreator;

			public:
				PaintImgOp(QT::RawAPI_OGLW * pOGLW);

			public:
				static const Basic::Ptr<PaintImgOp> New(QT::RawAPI_OGLW * pOGLW) {
					return Basic::New<PaintImgOp>(pOGLW);
				}

			public:
				bool SetOp(int w, int h);
				QT::RawAPI_OGLW * GetOGLW() { return pOGLW; }
				Basic::Ptr<Basic::Image> GetImg() { return img; }

			private:
				QT::RawAPI_OGLW * pOGLW;

				Basic::Ptr<Basic::Op> initOp;
				Basic::Ptr<Basic::Op> paintOp;
				Basic::Ptr<Basic::Op> resizeOp;
				Basic::Ptr<Basic::Image> img;

			private:
				virtual ~PaintImgOp() = default;
			};

			Basic::Ptr<PaintImgOp> GenScenePaintOp();
			
		private:
			QT::RawAPI_OGLW * pOGLW;
		};

	}
}
