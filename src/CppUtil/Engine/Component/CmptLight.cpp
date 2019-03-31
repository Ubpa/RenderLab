#include <CppUtil/Engine/CmptLight.h>

#include <CppUtil/Engine/CmptTransform.h>

#include <CppUtil/Basic/EleVisitor.h>

#include <CppUtil/Engine/SObj.h>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;

Transform CmptLight::GetLightToWorldMatrixWithoutScale() const {
	Transform tsfm(1.0f);
	auto sobj = GetSObj();
	if (!sobj)
		return tsfm;

	auto visitor = ToPtr(new EleVisitor);
	visitor->Reg<SObj>([&tsfm](SObj::Ptr sobj) {
		auto cmptTransform = sobj->GetComponent<CmptTransform>();
		if (!cmptTransform)
			return;

		auto pos = cmptTransform->GetPosition();
		auto rotation = cmptTransform->GetRotation();
		// tsfm = T * R * tsfm
		auto curTsfm = Transform::Translate(pos);
		curTsfm *= Transform::Rotate(rotation);
		curTsfm.MulTo(tsfm);
	});
	sobj->AscendAccept(visitor);

	return tsfm;
}