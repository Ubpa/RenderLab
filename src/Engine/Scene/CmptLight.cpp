#include <Engine/CmptLight.h>

#include <Engine/CmptTransform.h>

#include <Basic/Visitor.h>

#include <Engine/SObj.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

Ubpa::transformf CmptLight::GetLightToWorldMatrixWithoutScale() const {
	auto tsfm = Ubpa::transformf::eye();
	auto sobj = GetSObj();
	if (!sobj)
		return tsfm;

	auto visitor = Visitor::New();
	visitor->Reg([&tsfm](Ptr<SObj> sobj) {
		auto cmptTransform = sobj->GetComponent<CmptTransform>();
		if (!cmptTransform)
			return;

		auto pos = cmptTransform->GetPosition();
		auto rotation = cmptTransform->GetRotation();
		// tsfm = T * R * tsfm
		tsfm = Ubpa::transformf(pos.cast_to<Ubpa::vecf3>()) * Ubpa::transformf(rotation) * tsfm;
	});
	sobj->AscendAccept(visitor);

	return tsfm;
}
