#include <CppUtil/RTX/TexVisitor.h>

using namespace CppUtil;
using namespace RTX;

void TexVisitor::Visit(Basic::CPtr<Texture> tex) { }
void TexVisitor::Visit(Basic::CPtr<ConstTexture> constTexture) { }
void TexVisitor::Visit(Basic::CPtr<OpTexture> opTexture) { }
void TexVisitor::Visit(Basic::CPtr<ImgTexture> imgTexture) { }
void TexVisitor::Visit(Basic::CPtr<Skybox> skybox) { }
