#ifndef _ENGINE_COMPONENT_CMPT_MATERIAL_H_
#define _ENGINE_COMPONENT_CMPT_MATERIAL_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class MaterialBase;

		class CmptMaterial final : public Component<CmptMaterial> {
		public:
			CmptMaterial(Basic::Ptr<SObj> sobj, Basic::Ptr<MaterialBase> material)
				: Component(sobj), material(material) { }

		public:
			static const Basic::Ptr<CmptMaterial> New(Basic::Ptr<SObj> sobj, Basic::Ptr<MaterialBase> material) {
				return Basic::New<CmptMaterial>(sobj, material);
			}

		protected:
			virtual ~CmptMaterial() = default;

		public:
			Basic::Ptr<MaterialBase> material;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_MATERIAL_H_
