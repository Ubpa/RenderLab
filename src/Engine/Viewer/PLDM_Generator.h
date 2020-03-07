#pragma once

#include <OpenGL/FBO.h>
#include <OpenGL/Texture.h>
#include <OpenGL/VAO.h>
#include <OpenGL/Shader.h>

#include <Basic/HeapObj.h>

#include <UDP/Visitor.h>

#include <UGM/transform.h>

#include <map>

namespace Ubpa {
	class RawAPI_OGLW;

	class Scene;
	class SObj;

	class Primitive;
	class Sphere;
	class Plane;
	class TriMesh;
	class Disk;
	class Capsule;

	class CmptLight;

	class Light;
	class PointLight;
	class DirectionalLight;
	class SpotLight;

	// Point Light Depth Map Generator
	class PLDM_Generator : public HeapObj, public SharedPtrVisitor<PLDM_Generator, Primitive>{
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

		void Visit(Ptr<Scene> scene);
	protected:
		void Visit(Ptr<SObj> sobj);
		using SharedPtrVisitor<PLDM_Generator, Primitive>::Visit;

		void ImplVisit(Ptr<Sphere> sphere);
		void ImplVisit(Ptr<Plane> plane);
		void ImplVisit(Ptr<TriMesh> mesh);
		void ImplVisit(Ptr<Disk> disk);
		void ImplVisit(Ptr<Capsule> capsule);

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
