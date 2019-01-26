#ifndef _TEX_VISITOR_H_
#define _TEX_VISITOR_H_

#include <Utility/Basic/HeapObj.h>

namespace RayTracing {
	class Texture;
	class ConstTexture;
	class OpTexture;
	class ImgTexture;
	class Skybox;

	class TexVisitor : public CppUtility::Other::HeapObj {
		HEAP_OBJ_SETUP(TexVisitor)
	public:
		virtual void Visit(CppUtility::Other::CPtr<Texture> tex);
		virtual void Visit(CppUtility::Other::CPtr<ConstTexture> constTexture);
		virtual void Visit(CppUtility::Other::CPtr<OpTexture> opTexture);
		virtual void Visit(CppUtility::Other::CPtr<ImgTexture> imgTexture);
		virtual void Visit(CppUtility::Other::CPtr<Skybox> skybox);
	private:
	};
}

#endif // !_TEX_VISITOR_H_
