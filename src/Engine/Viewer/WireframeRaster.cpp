#include <Engine/WireframeRaster.h>

#include <Engine/AllBSDFs.h>

#include <Engine/Gooch.h>

#include <Qt/RawAPI_OGLW.h>
#include <Qt/RawAPI_Define.h>

#include <OpenGL/CommonDefine.h>
#include <OpenGL/Texture.h>
#include <OpenGL/Shader.h>

#include <Basic/Image.h>

#include <ROOT_PATH.h>

using namespace CppUtil::Engine;
using namespace CppUtil::QT;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace CppUtil;
using namespace Define;
using namespace std;

void WireframeRaster::Init() {
	ForwardRaster::Init();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	SetDrawSky(false);
}
