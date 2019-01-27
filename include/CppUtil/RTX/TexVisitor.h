#ifndef _TEX_VISITOR_H_
#define _TEX_VISITOR_H_

#include <CppUtil/Basic/HeapObj.h>

namespace RTX {
	class Texture;
	class ConstTexture;
	class OpTexture;
	class ImgTexture;
	class Skybox;

	class TexVisitor : public CppUtil::Basic::HeapObj {
		HEAP_OBJ_SETUP(TexVisitor)
	public:
		virtual void Visit(CppUtil::Basic::CPtr<Texture> tex);
		virtual void Visit(CppUtil::Basic::CPtr<ConstTexture> constTexture);
		virtual void Visit(CppUtil::Basic::CPtr<OpTexture> opTexture);
		virtual void Visit(CppUtil::Basic::CPtr<ImgTexture> imgTexture);
		virtual void Visit(CppUtil::Basic::CPtr<Skybox> skybox);
	private:
	};
}

#endif // !_TEX_VISITOR_H_
