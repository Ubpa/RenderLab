#ifndef _COMPONENT_MATERIAL_H_
#define _COMPONENT_MATERIAL_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class MaterialBase;

		class Material : public Component {
			COMPONENT_SETUP(Material)
		public:
			Material(Basic::Ptr<SObj> sobj = nullptr, Basic::Ptr<MaterialBase> material = nullptr)
				: Component(sobj), material(material) { }

			Basic::Ptr<MaterialBase> GetMat() const { return material; }
			void SetMat(Basic::Ptr<MaterialBase> material) { this->material = material; }

		private:
			Basic::Ptr<MaterialBase> material;
		};
	}
}

#endif//!_COMPONENT_MATERIAL_H_