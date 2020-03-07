#include <Engine/Scene/CmptLight.h>

#include <Engine/Scene/CmptTransform.h>

#include <UDP/Visitor.h>

#include <Engine/Scene/SObj.h>

using namespace Ubpa;

using namespace std;

transformf CmptLight::GetLightToWorldMatrixWithoutScale() const {
	auto tsfm = transformf::eye();

	for (auto cur = GetSObj(); cur != nullptr; cur = cur->GetParent()) {
		auto cmptTransform = cur->GetComponent<CmptTransform>();
		if (!cmptTransform)
			continue;

		auto pos = cmptTransform->GetPosition();
		auto rotation = cmptTransform->GetRotation();
		// tsfm = T * R * tsfm
		tsfm = transformf(pos.cast_to<vecf3>()) * transformf(rotation) * tsfm;
	}

	return tsfm;
}
