#ifndef _PAINT_IMG_OP_CREATOR_H_
#define _PAINT_IMG_OP_CREATOR_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class Image;
	}

	namespace Qt {
		class RawAPI_OGLW;

		class PaintImgOpCreator {
		public:
			PaintImgOpCreator(RawAPI_OGLW * pOGLW);

		public:
			class PaintImgOp : public Basic::HeapObj {
				HEAP_OBJ_SETUP(PaintImgOp)
					friend class PaintImgOpCreator;

			public:
				PaintImgOp(Qt::RawAPI_OGLW * pOGLW);
				bool SetOp(int w, int h);
				Qt::RawAPI_OGLW * GetOGLW() { return pOGLW; }
				Basic::Ptr<Basic::Image> GetImg() { return img; }

			private:
				Qt::RawAPI_OGLW * pOGLW;

				Basic::Ptr<Basic::Operation> initOp;
				Basic::Ptr<Basic::Operation> paintOp;
				Basic::Ptr<Basic::Operation> resizeOp;
				Basic::Ptr<Basic::Image> img;
			};

			PaintImgOp::Ptr GenScenePaintOp();
			
		private:
			Qt::RawAPI_OGLW * pOGLW;
		};

	}
}


#endif // !_PAINT_IMG_OP_CREATOR_H_
