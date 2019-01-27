#include <CppUtil/RTX/Group.h>

using namespace RTX;
using namespace CppUtil::Basic;

Group::Group(Material::CPtr material)
	: Hitable(material), box(AABB::InValid) { }

HitRst Group::RayIn(CppUtil::Basic::Ptr<Ray> & ray) const {
	HitRst finalHitRst = HitRst::InValid;

	for (size_t i = 0; i < children.size(); i++) {
		Hitable::CPtr hitable = children[i];
		HitRst hitRst = hitable->RayIn(ray);

		if (hitRst.hit)
			finalHitRst = hitRst;

	}

	if (finalHitRst.hit && finalHitRst.isMatCoverable && GetMat() != NULL) {
		finalHitRst.material = GetMat();
		finalHitRst.isMatCoverable = IsMatCoverable();
	}

	return finalHitRst;
}

Group & Group::operator <<(Hitable::CPtr hitable) {
	if (hitable != NULL) {
		children.push_back(hitable);
		box += hitable->GetBoundingBox();
	}
	
	return *this;
}