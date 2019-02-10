#ifndef _BASIC_EVENT_MANAGER_EVENT_MANAGER_H_
#define _BASIC_EVENT_MANAGER_EVENT_MANAGER_H_

#include <CppUtil/Basic/Ptr.h>
#include <CppUtil/Basic/Singleton.h>

#include <functional>
#include <map>
#include <tuple>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class OpQueue;

		class EventMngr : public Singleton<EventMngr> {
		public:
			enum ENUM_EVENT_TYPE {
				KB_PRESS,
				KB_RELEASE,
				MOUSE_MOVE,
				MOUSE_PRESS,
				MOUSE_RELEASE,
				MOUSE_WHEEL,
			};
			//------------
			friend class Singleton<EventMngr>;
		private:
			static EventMngr * GetInstance();
			//------------
			void _Reg(size_t event, Ptr<Operation> op);
			void _Reg(size_t event, void * target, Ptr<Operation> op);
			void _Reg(size_t event, void * target, ENUM_EVENT_TYPE eventType, Ptr<Operation> op);

			void _Response(size_t event);
			void _Response(size_t event, void * target);
			void _Response(size_t event, void * target, ENUM_EVENT_TYPE eventType);
		public:
			static void Reg(size_t event, Ptr<Operation> op) { GetInstance()->_Reg(event, op); }
			static void Reg(size_t event, void * target, Ptr<Operation> op) { GetInstance()->_Reg(event, target, op); }
			static void Reg(size_t event, void * target, ENUM_EVENT_TYPE eventType, Ptr<Operation> op) { GetInstance()->_Reg(event, target, eventType, op); }

			static void Response(size_t event) { GetInstance()->_Response(event); }
			static void Response(size_t event, void * target) { GetInstance()->_Response(event, target); }
			static void Response(size_t event, void * target, ENUM_EVENT_TYPE eventType) { GetInstance()->_Response(event, target, eventType); }

		protected:
			EventMngr() = default;
			~EventMngr() = default;

		private:
			//------------
			std::map<size_t, Ptr<OpQueue> > directory;
			std::map<std::tuple<size_t, void*>, Ptr<OpQueue> > directory2;
			std::map<std::tuple<size_t, void*, ENUM_EVENT_TYPE>, Ptr<OpQueue> > directory3;
		};
	}
}
#endif // !_BASIC_EVENT_MANAGER_EVENT_MANAGER_H_
