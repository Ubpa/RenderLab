#ifndef _RASTER_SCENE_CREATOR_H_
#define _RASTER_SCENE_CREATOR_H_

#include <CppUtil/Basic/Operation.h>
#include <CppUtil/Basic/HeapObj.h>

namespace CppUtil {
	namespace Qt {
		class RawAPI_OGLW;
	}
}

class RasterSceneCreator {
public:
	RasterSceneCreator(CppUtil::Qt::RawAPI_OGLW * pOGLW);

	class SceneOp : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(SceneOp)
		friend class RasterSceneCreator;

	public:
		SceneOp(CppUtil::Qt::RawAPI_OGLW * pOGLW);
		bool SetOp();
		CppUtil::Qt::RawAPI_OGLW * GetOGLW() { return pOGLW; }

	private:
		CppUtil::Basic::Operation::Ptr GetDefaultResizeOp();
		CppUtil::Basic::Operation::Ptr GetDefaultListenerInitOp();

	private:
		CppUtil::Qt::RawAPI_OGLW * pOGLW;

		CppUtil::Basic::Operation::Ptr initOp;
		CppUtil::Basic::Operation::Ptr paintOp;
		CppUtil::Basic::Operation::Ptr resizeOp;
		CppUtil::Basic::Operation::Ptr listenerInitOp;
	};

	SceneOp::Ptr GenScenePaintOp(int n);

private:
	SceneOp::Ptr GenScenePaintOp_0();
	SceneOp::Ptr GenScenePaintOp_1();


	CppUtil::Qt::RawAPI_OGLW * pOGLW;
};

#endif // !_RASTER_SCENE_CREATOR_H_
