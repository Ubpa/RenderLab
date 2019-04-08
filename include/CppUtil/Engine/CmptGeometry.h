#ifndef _ENGINE_COMPONENT_CMPT_GEOMETRY_H_
#define _ENGINE_COMPONENT_CMPT_GEOMETRY_H_

#include <CppUtil/Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class PrimitiveBase;

		class CmptGeometry final : public Component<CmptGeometry> {
		public:
			CmptGeometry(Basic::Ptr<SObj> sobj, Basic::Ptr<PrimitiveBase> primitive)
				: Component(sobj), primitive(primitive) { }

		public:
			static const Basic::Ptr<CmptGeometry> New(Basic::Ptr<SObj> sobj, Basic::Ptr<PrimitiveBase> primitive) {
				return Basic::New<CmptGeometry>(sobj, primitive);
			}

		protected:
			virtual ~CmptGeometry() = default;

		public:
			Basic::Ptr<PrimitiveBase> primitive;
		};
	}
}

#endif//!_ENGINE_COMPONENT_CMPT_GEOMETRY_H_