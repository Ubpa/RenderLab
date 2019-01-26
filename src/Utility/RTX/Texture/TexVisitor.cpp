#include <Utility/RTX/TexVisitor.h>

using namespace CppUtility;
using namespace RayTracing;

void TexVisitor::Visit(Other::CPtr<Texture> tex) { }
void TexVisitor::Visit(Other::CPtr<ConstTexture> constTexture) { }
void TexVisitor::Visit(Other::CPtr<OpTexture> opTexture) { }
void TexVisitor::Visit(Other::CPtr<ImgTexture> imgTexture) { }
void TexVisitor::Visit(Other::CPtr<Skybox> skybox) { }
