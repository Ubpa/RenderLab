#include <CppUtil/Engine/Raster.h>

#include "Impl_Raster.h"

using namespace CppUtil::Engine;
using namespace CppUtil::Qt;
using namespace CppUtil::OpenGL;

Raster::Raster(RawAPI_OGLW * pOGLW, CppUtil::Basic::Ptr<Scene> scene)
	: pImpl(ToPtr(new Impl_Raster(pOGLW, scene))) { }

Shader & Raster::GetShader() const {
	return pImpl->shader_basic;
}

void Raster::Draw() {
	pImpl->Draw();
}

void Raster::Init() {
	pImpl->Init();
}
