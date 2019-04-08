#ifndef _BASIC_EVENT_MANAGER_EVENT_MANAGER_H_
#define _BASIC_EVENT_MANAGER_EVENT_MANAGER_H_

#include <CppUtil/Basic/Ptr.h>

#include <functional>
#include <map>
#include <tuple>

namespace CppUtil {
	namespace Basic {
		class Op;
		class OpQueue;

		class EventMngr {
		public:
			enum ENUM_EVENT_TYPE {
				KB_PRESS,
				KB_RELEASE,
				MOUSE_MOVE,
				MOUSE_PRESS,
				MOUSE_RELEASE,
				MOUSE_WHEEL,
			};

		public:
			static EventMngr & GetInstance() {
				static EventMngr instance;
				return instance;
			}

			void Reg(size_t event, Ptr<Op> op);
			void Reg(size_t event, ENUM_EVENT_TYPE eventType, Ptr<Op> op);
			void Reg(size_t event, void * target, Ptr<Op> op);
			void Reg(size_t event, void * target, ENUM_EVENT_TYPE eventType, Ptr<Op> op);

			void Response(size_t event);
			void Response(size_t event, ENUM_EVENT_TYPE eventType);
			void Response(size_t event, void * target);
			void Response(size_t event, void * target, ENUM_EVENT_TYPE eventType);

		protected:
			EventMngr() = default;
			~EventMngr() = default;

		private:
			std::map<size_t, Ptr<OpQueue> > directory;

			std::map<std::tuple<size_t, void*>, Ptr<OpQueue> > directory20;
			std::map<std::tuple<size_t, ENUM_EVENT_TYPE>, Ptr<OpQueue> > directory21;

			std::map<std::tuple<size_t, void*, ENUM_EVENT_TYPE>, Ptr<OpQueue> > directory3;
		};
	}
}
#endif // !_BASIC_EVENT_MANAGER_EVENT_MANAGER_H_
