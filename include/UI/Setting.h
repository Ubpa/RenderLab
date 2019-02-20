#ifndef _UI_SETTING_SETTING_H_
#define _UI_SETTING_SETTING_H_

#include <UI/Grid.h>

namespace Ui {
	class Setting : public Grid {
		HEAP_OBJ_SETUP(Setting)
	public:
		static Setting::Ptr GetInstance() {
			static auto instance = ToPtr(new Setting);
			return instance;
		}
	private:
		Setting() = default;
	};
}

#endif//!_UI_SETTING_SETTING_H_
