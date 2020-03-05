#pragma once

#include <Basic/Visitor.h>
#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <UGM/transform.h>

namespace Ubpa {
	class RawAPI_OGLW;
	class Camera;

	class Scene;
	class SObj;

	class Sphere;
	class Plane;
	class TriMesh;
	class Disk;
	class Capsule;

	class CmptLight;

	// Spot Light Depth Map Generator
	class SLDM_Generator : public Visitor {
	public:
		SLDM_Generator(RawAPI_OGLW* pOGLW, Ptr<Camera> camera, float lightNear, float lightFar);

	public:
		static const Ptr<SLDM_Generator> New(RawAPI_OGLW* pOGLW, Ptr<Camera> camera, float lightNear, float lightFar) {
			return Ubpa::New<SLDM_Generator>(pOGLW, camera, lightNear, lightFar);
		}

	protected:
		virtual ~SLDM_Generator() = default;

	public:
		void Init();
		const Texture GetDepthMap(PtrC<CmptLight> light) const;
		const transformf GetProjView(PtrC<CmptLight> light) const;

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
		Ptr<Camera> camera;

		struct FBO_Tex {
			FBO_Tex(const FBO& fbo = FBO(), const Texture& tex = Texture())
				: fbo(fbo), tex(tex) { }

			FBO fbo;
			Texture tex;
		};
		std::map<WPtrC<CmptLight>, FBO_Tex> lightMap;
		std::map<WPtrC<CmptLight>, transformf> light2pv;
		int depthMapSize;
		float lightNear;
		float lightFar;

		Shader shader_genDepth;
		std::vector<transformf> modelVec;
	};
}
