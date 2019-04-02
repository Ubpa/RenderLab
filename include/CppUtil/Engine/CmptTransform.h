#ifndef _ENGINE_COMPONENT_CMPT_TRANSFORM_H_
#define _ENGINE_COMPONENT_CMPT_TRANSFORM_H_

#include <CppUtil/Engine/Component.h>
#include <CppUtil/Basic/Transform.h>

namespace CppUtil {
	namespace Engine {
		class CmptTransform : public Component {
			COMPONENT_SETUP(CmptTransform)
		public:
			CmptTransform(Basic::Ptr<SObj> sobj);

			const Point3 & GetPosition() const { return position; }
			const Quatf & GetRotation() const { return rotation; }
			const Vec3 & GetScale() const { return scale; }
			const Basic::Transform & GetTransform();

			void SetPosition(const Point3 & position);
			void SetRotation(const Quatf & rotation);
			void SetScale(const Vec3 & xyz);
			void SetTransform(const Basic::Transform & transform);

		private:
			void UpdateMat();

			Point3 position;
			Quatf rotation;
			Vec3 scale;

			bool dirtyTransform;
			Basic::Transform transform;
		};
	}
}

#endif // !_ENGINE_COMPONENT_CMPT_TRANSFORM_H_
