#pragma once

#include <Engine/Scene/Component.h>

#include <UGM/transform.h>

namespace Ubpa {
	class Light;

	class CmptLight final : public Component {
	public:
		CmptLight(Ptr<SObj> sobj, Ptr<Light> light)
			: Component(sobj), light(light) { }

	public:
		static const Ptr<CmptLight> New(Ptr<SObj> sobj, Ptr<Light> light) {
			return Ubpa::New<CmptLight>(sobj, light);
		}

	protected:
		virtual ~CmptLight() = default;

	public:
		transformf GetLightToWorldMatrixWithoutScale() const;

	public:
		Ptr<Light> light;
	};
}
