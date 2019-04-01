#ifndef _ENGINE_COMPONENT_CMPT_MATERIAL_H_
#define _ENGINE_COMPONENT_CMPT_MATERIAL_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class Material;

		class CmptMaterial : public Component {
			COMPONENT_SETUP(CmptMaterial)
		public:
			CmptMaterial(Basic::Ptr<SObj> sobj, Basic::Ptr<Material> material)
				: Component(sobj), material(material) { }

		public:
			Basic::Ptr<Material> material;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_MATERIAL_H_
