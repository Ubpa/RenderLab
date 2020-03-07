#pragma once

#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <Basic/HeapObj.h>

#include <UGM/transform.h>

#include <UDP/Visitor.h>

#include <map>

namespace Ubpa {
	class RawAPI_OGLW;
	class Camera;

	class Scene;
	class SObj;

	class Primitive;
	class Sphere;
	class Plane;
	class TriMesh;
	class Disk;
	class Capsule;

	class CmptLight;

	// Spot Light Depth Map Generator
	class SLDM_Generator final : public HeapObj, public SharedPtrVisitor<SLDM_Generator, Primitive> {
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

		void Visit(Ptr<Scene> scene);
	protected:
		void Visit(Ptr<SObj> sobj);
		using SharedPtrVisitor<SLDM_Generator, Primitive>::Visit;

		void ImplVisit(Ptr<Sphere> sphere);
		void ImplVisit(Ptr<Plane> plane);
		void ImplVisit(Ptr<TriMesh> mesh);
		void ImplVisit(Ptr<Disk> disk);
		void ImplVisit(Ptr<Capsule> capsule);

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
