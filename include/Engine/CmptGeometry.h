#pragma once

#include <Engine/Component.h>

namespace CppUtil {
	namespace Engine {
		class Primitive;

		class CmptGeometry final : public Component {
		public:
			CmptGeometry(Basic::Ptr<SObj> sobj, Basic::Ptr<Primitive> primitive)
				: Component(sobj), primitive(primitive) { }

		public:
			static const Basic::Ptr<CmptGeometry> New(Basic::Ptr<SObj> sobj, Basic::Ptr<Primitive> primitive) {
				return Basic::New<CmptGeometry>(sobj, primitive);
			}

		protected:
			virtual ~CmptGeometry() = default;

		public:
			Basic::Ptr<Primitive> primitive;
		};
	}
}
