#pragma once

#include <Basic/Visitor.h>
#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <UGM/transform.h>

namespace Ubpa {
	class RawAPI_OGLW;

	class Scene;
	class SObj;

	class Sphere;
	class Plane;
	class TriMesh;
	class Disk;
	class Capsule;

	class CmptLight;

	class PointLight;
	class DirectionalLight;
	class SpotLight;

	// Point Light Depth Map Generator
	class PLDM_Generator : public Visitor {
	public:
		PLDM_Generator(RawAPI_OGLW* pOGLW, float lightNear, float lightFar);

	public:
		static const Ptr<PLDM_Generator> New(RawAPI_OGLW* pOGLW, float lightNear, float lightFar) {
			return Ubpa::New<PLDM_Generator>(pOGLW, lightNear, lightFar);
		}

	protected:
		virtual ~PLDM_Generator() = default;

	public:
		void Init();
		const Texture GetDepthCubeMap(PtrC<CmptLight> light) const;

	private:
		void Visit(Ptr<Scene> scene);
		void Visit(Ptr<SObj> sobj);

		void Visit(Ptr<Sphere> sphere);
		void Visit(Ptr<Plane> plane);
		void Visit(Ptr<TriMesh> mesh);
		void Visit(Ptr<Disk> disk);
		void Visit(Ptr<Capsule> capsule);

	private:
		RawAPI_OGLW* pOGLW;

		struct FBO_Tex {
			FBO_Tex(const FBO& fbo = FBO(), const Texture& tex = Texture())
				: fbo(fbo), tex(tex) { }

			FBO fbo;
			Texture tex;
		};
		std::map<WPtrC<CmptLight>, FBO_Tex> lightMap;
		int depthMapSize;
		float lightNear;
		float lightFar;

		Shader shader_genDepth;

		std::vector<transformf> modelVec;
	};
}
