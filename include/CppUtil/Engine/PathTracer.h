#ifndef _ENGINE_RTX_PATH_TRACER_H_
#define _ENGINE_RTX_PATH_TRACER_H_

#include <CppUtil/Engine/RayTracer.h>
#include <CppUtil/Engine/BBox.h>

#include <glm/mat3x3.hpp>

#include <vector>
#include <map>

namespace CppUtil {
	namespace Basic {
		class Element;
	}

	namespace Engine {
		class SObj;
		class LightBase;
		class Primitive;
		template <typename T, typename HolderT>
		class BVHNode;

		class PathTracer : public RayTracer {
			HEAP_OBJ_SETUP(PathTracer)
		public:
			PathTracer(Basic::Ptr<Scene> scene);

			virtual void Init();

			Basic::Ptr<BVHNode<Basic::Element, PathTracer>> GetBVHRoot() const { return bvhRoot; }
			const glm::mat4 & GetEleW2LMat(Basic::Ptr<Basic::Element> element);
			const glm::mat4 & GetEleL2WMat(Basic::Ptr<Basic::Element> element);
			const BBox & GetBBox(Basic::Ptr<Basic::Element> ele) { return ele2bbox[ele]; }
			const Basic::Ptr<SObj> GetSObj(Basic::Ptr<Basic::Element> ele);

		protected:
			// ray 处于世界坐标系
			virtual glm::vec3 Trace(Basic::Ptr<Ray> ray, int depth);
		
		public:
			int sampleNumForAreaLight;
			int maxDepth;

		private:
			std::vector<Basic::Ptr<LightBase>> lights;
			std::vector<glm::mat4> worldToLightVec;
			std::vector<glm::mat3> dir_worldToLightVec;// 只需要旋转方向，所以使用 mat3
			std::vector<glm::mat3> dir_lightToWorldVec;// 只需要旋转方向，所以使用 mat3

			std::map<Basic::Ptr<SObj>, glm::mat3> norm_localToWorld;

			// bvh
			Basic::Ptr<BVHNode<Basic::Element, PathTracer>> bvhRoot;
			// triangle 要通过 mesh 在这里取得 matrix
			class GetPrimitiveVisitor;
			std::map<Basic::Ptr<Primitive>, glm::mat4> worldToLocalMatrixes;
			std::map<Basic::Ptr<Primitive>, glm::mat4> localToWorldMatrixes;
			std::map<Basic::Ptr<Primitive>, Basic::Ptr<SObj>> primitive2sobj;

			// obj and box
			class BVHInitVisitor;
			friend class BVHInitVisitor;
			std::vector<Basic::Ptr<Basic::Element>> elements;
			std::map<Basic::Ptr<Basic::Element>, BBox> ele2bbox;
		};
	}
}

#endif//!_ENGINE_PATH_TRACER_H_
