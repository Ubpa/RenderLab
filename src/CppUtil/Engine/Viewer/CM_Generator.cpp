#include "CM_Generator.h"

#include <CppUtil/Qt/RawAPI_Define.h>
#include <CppUtil/Qt/RawAPI_OGLW.h>

#include <CppUtil/Engine/Scene.h>
#include <CppUtil/Engine/SObj.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Engine/CmptLight.h>
#include <CppUtil/Engine/InfiniteAreaLight.h>

#include <CppUtil/Engine/CmptGeometry.h>
#include <CppUtil/Engine/Sphere.h>
#include <CppUtil/Engine/Plane.h>

#include <CppUtil/OpenGL/Camera.h>

#include <ROOT_PATH.h>

using namespace CppUtil;
using namespace CppUtil::QT;
using namespace CppUtil::Engine;
using namespace CppUtil::OpenGL;
using namespace CppUtil::Basic;
using namespace std;

const string rootPath = ROOT_PATH;

namespace CppUtil {
	namespace Engine {
		const string str_PointLight_prefix = "data/shaders/Engine/PointLight/";
		const string str_genDepth = "genDepth";
		const string str_genDepth_vs = str_PointLight_prefix + str_genDepth + ".vs";
		const string str_genDepth_gs = str_PointLight_prefix + str_genDepth + ".gs";
		const string str_genDepth_fs = str_PointLight_prefix + str_genDepth + ".fs";
	}
}

CM_Generator::CM_Generator(QT::RawAPI_OGLW * pOGLW)
	: pOGLW(pOGLW), mapSize(1024)
{
	RegMemberFunc<Scene>(&CM_Generator::Visit);
}

void CM_Generator::OGL_Init() {
	captureFBO = FBO(mapSize, mapSize, FBO::ENUM_TYPE_DYNAMIC_COLOR);

	string vsPath = "data/shaders/Engine/Cubemap/cubemap.vs";
	string fsPath = "data/shaders/Engine/Cubemap/equirectangular_to_cubemap.fs";
	shader_genCubemap = Shader(ROOT_PATH + vsPath, ROOT_PATH + fsPath);

	shader_genCubemap.SetInt("equirectangularMap", 0);

	auto captureProjection = Transform::Perspcetive(90.f, 1.0f, 0.1f, 10.0f);
	shader_genCubemap.SetMat4f("projection", captureProjection.GetMatrix().Data());
}

void CM_Generator::Visit(Ptr<Scene> scene) {
	if (!scene || !scene->GetRoot()) {
		printf("ERROR::CM_Generator::Visit(Ptr<Scene> scene):\n"
			"\t""scene or scene's root is nullptr\n");
		return;
	}

	// debug
	// img2tex.clear();

	// clear useless item
	auto nextIt = img2tex.begin();
	while (nextIt != img2tex.end()) {
		auto curIt = nextIt;
		nextIt++;

		if (curIt->first.expired())
			img2tex.erase(curIt);
	}

	scene->SetWriteLock(true);

	GLint origFBO;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &origFBO);
	GLint origViewport[4];
	glGetIntegerv(GL_VIEWPORT, origViewport);

	Transform captureViews[] = {
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(-1.0f,  0.0f,  0.0f), Vec3(0.0f, -1.0f,  0.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  1.0f,  0.0f), Vec3(0.0f,  0.0f,  1.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, -1.0f,  0.0f), Vec3(0.0f,  0.0f, -1.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f,  1.0f), Vec3(0.0f, -1.0f,  0.0f)),
		Transform::LookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f,  0.0f, -1.0f), Vec3(0.0f, -1.0f,  0.0f))
	};

	FBO::TexTarget mapper[6] = {
		FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_X,
		FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_X,
		FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_Y,
		FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_Y,
		FBO::TexTarget::TEXTURE_CUBE_MAP_POSITIVE_Z,
		FBO::TexTarget::TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	for (auto cmptLight : scene->GetCmptLights()) {
		auto infiniteAreaLight = CastTo<InfiniteAreaLight>(cmptLight->light);
		if (!infiniteAreaLight)
			continue;

		auto img = infiniteAreaLight->GetImg();
		if (!img || img2tex.find(img) != img2tex.end())
			continue;

		Texture imgTex(img);
		
		Texture cubemap(Texture::ENUM_TYPE_CUBE_MAP);
		cubemap.GenBufferForCubemap(mapSize, mapSize);

		img2tex[img] = cubemap;

		captureFBO.Use();
		glViewport(0, 0, mapSize, mapSize);

		imgTex.Use(0);
		
		for (int i = 0; i < 6; i++) {
			shader_genCubemap.SetMat4f("view", captureViews[i]);
			captureFBO.SetColor(cubemap, mapper[i]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			pOGLW->GetVAO(ShapeType::Cube).Draw(shader_genCubemap);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, origFBO);
	glViewport(origViewport[0], origViewport[1], origViewport[2], origViewport[3]);

	scene->SetWriteLock(false);
}

const Texture CM_Generator::GetCubeMap(PtrC<Image> img) const {
	auto target = img2tex.find(img);
	if (target == img2tex.end())
		return Texture::InValid;

	return target->second;
}
