#include <CppUtil/Engine/CmptLight.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Basic/Visitor.h>

#include <CppUtil/Engine/SObj.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace std;

Transform CmptLight::GetLightToWorldMatrixWithoutScale() const {
	Transform tsfm(1.0f);
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
		tsfm = Transform::Translate(pos) * Transform::Rotate(rotation) * tsfm;
	});
	sobj->AscendAccept(visitor);

	return tsfm;
}
