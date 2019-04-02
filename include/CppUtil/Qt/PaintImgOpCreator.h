#ifndef _QT_PAINT_IMG_OP_CREATOR_PAINT_IMG_OP_CREATOR_H_
#define _QT_PAINT_IMG_OP_CREATOR_PAINT_IMG_OP_CREATOR_H_

#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class Image;
	}

	namespace QT {
		class RawAPI_OGLW;

		class PaintImgOpCreator {
		public:
			PaintImgOpCreator(RawAPI_OGLW * pOGLW);

		public:
			class PaintImgOp : public Basic::HeapObj {
				HEAP_OBJ_SETUP(PaintImgOp)
					friend class PaintImgOpCreator;

			public:
				PaintImgOp(QT::RawAPI_OGLW * pOGLW);
				bool SetOp(int w, int h);
				QT::RawAPI_OGLW * GetOGLW() { return pOGLW; }
				Basic::Ptr<Basic::Image> GetImg() { return img; }

			private:
				QT::RawAPI_OGLW * pOGLW;

				Basic::Ptr<Basic::Operation> initOp;
				Basic::Ptr<Basic::Operation> paintOp;
				Basic::Ptr<Basic::Operation> resizeOp;
				Basic::Ptr<Basic::Image> img;
			};

			PaintImgOp::Ptr GenScenePaintOp();
			
		private:
			QT::RawAPI_OGLW * pOGLW;
		};

	}
}


#endif // !_QT_PAINT_IMG_OP_CREATOR_PAINT_IMG_OP_CREATOR_H_
