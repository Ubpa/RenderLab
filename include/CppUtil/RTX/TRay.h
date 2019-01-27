#ifndef _TRAY_H_
#define _TRAY_H_

#include <CppUtil/RTX/Ray.h>

namespace RTX {
	class TRay : public Ray {
		HEAP_OBJ_SETUP(TRay)
	public:
		TRay(const glm::vec3 & origin = glm::vec3(0), const glm::vec3 & dir = glm::vec3(1), float time = 0.0f);

		void SetTime(float time);
		float GetTime() const;

		virtual Ray::Ptr GenCopy() const;
	protected:
		void CopyTo(TRay::Ptr tRay) const;

		float time;
	};
}
#endif //!_TRAY_H_