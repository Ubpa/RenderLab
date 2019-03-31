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

			const Pointf & GetPosition() const { return position; }
			const Quatf & GetRotation() const { return rotation; }
			const Vectorf & GetScale() const { return scale; }
			const Basic::Transform & GetTransform();

			void SetPosition(const Pointf & position);
			void SetRotation(const Quatf & rotation);
			void SetScale(const Vectorf & xyz);
			void SetTransform(const Basic::Transform & transform);

		private:
			void UpdateMat();

			Pointf position;
			Quatf rotation;
			Vectorf scale;

			bool dirtyTransform;
			Basic::Transform transform;
		};
	}
}

#endif // !_ENGINE_COMPONENT_CMPT_TRANSFORM_H_
