#ifndef _RTX_HITABLE_SKY_H_
#define _RTX_HITABLE_SKY_H_

#include <CppUtil/RTX/Hitable.h>

namespace RTX {
	class Sky : public Hitable {
		HITABLE_SETUP(Sky)
	public:
		using Hitable::Hitable;

		virtual HitRst RayIn(CppUtil::Basic::Ptr<Ray> & ray) const;

		virtual const AABB GetBoundingBox() const { return box; }
	private:
		static const AABB box;
	};
}

#endif // !_RTX_HITABLE_SKY_H_
