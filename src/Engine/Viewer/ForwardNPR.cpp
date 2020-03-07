#include <Engine/Viewer/ForwardNPR.h>

#include <Engine/Material/AllBSDFs.h>

#include <Engine/Material/Gooch.h>

#include <Qt/RawAPI_OGLW.h>
#include <Qt/RawAPI_Define.h>

#include <OpenGL/CommonDefine.h>
#include <OpenGL/Texture.h>
#include <OpenGL/Shader.h>

#include <Basic/Image.h>

#include <ROOT_PATH.h>

using namespace Ubpa;

using namespace Define;
using namespace std;

void ForwardNPR::Init() {
	ForwardRaster::Init();

	InitShaders();
}

void ForwardNPR::InitShaders() {
	InitShaderGooch();
}

void ForwardNPR::InitShaderGooch() {
	shaderGooch = Shader(ROOT_PATH + str_Basic_P3N3T2T3_vs, ROOT_PATH + "data/shaders/Engine/Gooch.fs");

	shaderGooch.SetInt("gooch.colorTexture", 0);

	RegShader(shaderGooch, 1);
}

void ForwardNPR::ImplVisit(Ptr<Gooch> gooch) {
	SetCurShader(shaderGooch);

	shaderGooch.SetVecf3("gooch.colorFactor", gooch->colorFactor.cast_to<valf3>());
	if (gooch->colorTexture && gooch->colorTexture->IsValid()) {
		shaderGooch.SetBool("gooch.haveColorTexture", true);
		pOGLW->GetTex(gooch->colorTexture).Use(0);
	}
	else
		shaderGooch.SetBool("gooch.haveColorTexture", false);

	UseLightTex(shaderGooch);
}
