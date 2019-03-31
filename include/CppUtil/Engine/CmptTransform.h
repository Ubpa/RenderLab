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

			const Basic::Pointf & GetPosition() const { return position; }
			const Basic::Quatf & GetRotation() const { return rotation; }
			const Basic::Val3f & GetScale() const { return scale; }
			const Basic::Transform & GetTransform();

			void SetPosition(const Basic::Pointf & position);
			void SetRotation(const Basic::Quatf & rotation);
			void SetScale(const Basic::Val3f & xyz);
			void SetTransform(const Basic::Transform & transform);

		private:
			void UpdateMat();

			Basic::Pointf position;
			Basic::Quatf rotation;
			Basic::Val3f scale;

			bool dirtyTransform;
			Basic::Transform transform;
		};
	}
}

#endif // !_ENGINE_COMPONENT_CMPT_TRANSFORM_H_
