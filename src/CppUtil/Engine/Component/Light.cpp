#include <CppUtil/Engine/Light.h>

#include <CppUtil/Engine/Transform.h>

#include <CppUtil/Basic/EleVisitor.h>

#include <CppUtil/Engine/SObj.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace CppUtil::Engine;
using namespace CppUtil::Basic;
using namespace glm;

mat4 Light::GetLightToWorldMatrixWithoutScale() const {
	mat4 mat(1.0f);
	auto sobj = GetSObj();
	if (!sobj)
		return mat;

	auto visitor = ToPtr(new EleVisitor);
	visitor->Reg<SObj>([&mat](SObj::Ptr sobj) {
		auto transform = sobj->GetComponent<Transform>();
		if (!transform)
			return;

		auto pos = transform->GetPosition();
		auto rotation = transform->GetRotation();
		mat4 curMat(1.0f);
		// mat = T * R * mat
		curMat = translate(curMat, pos);
		mat = curMat * mat4_cast(rotation) * mat;
	});
	sobj->AscendAccept(visitor);

	return mat;
}