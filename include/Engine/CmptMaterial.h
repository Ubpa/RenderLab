#ifndef _ENGINE_COMPONENT_CMPT_MATERIAL_H_
#define _ENGINE_COMPONENT_CMPT_MATERIAL_H_

#include <Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class Material;

		class CmptMaterial final : public Component {
		public:
			CmptMaterial(Basic::Ptr<SObj> sobj, Basic::Ptr<Material> material)
				: Component(sobj), material(material) { }

		public:
			static const Basic::Ptr<CmptMaterial> New(Basic::Ptr<SObj> sobj, Basic::Ptr<Material> material) {
				return Basic::New<CmptMaterial>(sobj, material);
			}

		protected:
			virtual ~CmptMaterial() = default;

		public:
			Basic::Ptr<Material> material;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_MATERIAL_H_
