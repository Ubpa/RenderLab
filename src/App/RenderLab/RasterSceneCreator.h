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
		SceneOp(CppUtil::Qt::RawAPI_OGLW * pOGLW, CppUtil::Basic::Operation::Ptr initOp = nullptr, CppUtil::Basic::Operation::Ptr paintOp = nullptr);
		bool SetOp();
	private:
		CppUtil::Qt::RawAPI_OGLW * pOGLW;
		CppUtil::Basic::Operation::Ptr initOp;
		CppUtil::Basic::Operation::Ptr paintOp;
	};

	SceneOp::Ptr GenScenePaintOp(int n);

private:
	SceneOp::Ptr GenScenePaintOp_0();


	CppUtil::Qt::RawAPI_OGLW * pOGLW;
};

#endif // !_RASTER_SCENE_CREATOR_H_
