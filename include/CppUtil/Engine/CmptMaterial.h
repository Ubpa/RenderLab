#ifndef _ENGINE_COMPONENT_CMPT_MATERIAL_H_
#define _ENGINE_COMPONENT_CMPT_MATERIAL_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class Material;

		class Material : public Component {
			COMPONENT_SETUP(Material)
		public:
			Material(Basic::Ptr<SObj> sobj, Basic::Ptr<Material> material)
				: Component(sobj), material(material) { }

			Basic::Ptr<Material> GetMat() const { return material; }
			void SetMat(Basic::Ptr<Material> material) { this->material = material; }

		private:
			Basic::Ptr<Material> material;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_MATERIAL_H_