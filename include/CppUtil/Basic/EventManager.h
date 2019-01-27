#ifndef _EVENT_MANAGER_H_
#define _EVENT_MANAGER_H_

#include <CppUtil/Basic/Ptr.h>
#include <CppUtil/Basic/Singleton.h>

#include <functional>
#include <map>

namespace CppUtil {
	namespace Basic {
		class Operation;
		class OpQueue;

		class EventManager : public Singleton<EventManager> {
		public:
			enum ENUM_EVENT {
				//KEY = 0x00000000 ~ 0x0000FFFF
				KEYBOARD_PRESS = 0x00010000,
				KEYBOARD_REPEAT = 0x00020000,
				KEYBOARD_RELEASE = 0x00040000,
				KEYBOARD = KEYBOARD_PRESS | KEYBOARD_REPEAT | KEYBOARD_RELEASE,
				MOUSE_SCROLL = 0x00080000,
				MOUSE_MOUVE = 0x00080001,
				WINDOW_ZOOM = 0x00080002,
			};
			//------------
			friend class Singleton<EventManager>;
			static EventManager * GetInstance();
			//------------
			void Register(size_t event, Ptr<Operation> op);
			void Register(size_t event, const std::function<void()> & op);
			void Response(size_t event);
		protected:
			EventManager() = default;
			~EventManager() = default;
			//------------
			std::map<size_t, Ptr<OpQueue> > directory;
		};
	}
}
#endif // !_EVENT_MANAGER_H_
