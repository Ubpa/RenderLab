#include <CppUtil/RTX/Hitable.h>
#include <CppUtil/Basic/Math.h>

using namespace RTX;
using namespace CppUtil::Basic;
using namespace glm;
using namespace std;

const HitRst HitRst::InValid(false);

Hitable::Hitable(Material::CPtr material)
	: material(material) {
	isMatCoverable = material ? false : true;
}