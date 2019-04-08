#ifndef _UI_SETTING_SETTING_H_
#define _UI_SETTING_SETTING_H_

#include <UI/Grid.h>

namespace Ui {
	class Setting : public Grid {
	public:
		using Grid::Grid;

	protected:
		virtual ~Setting() = default;

	public:
		static CppUtil::Basic::Ptr<Setting> GetInstance() {
			static auto instance = CppUtil::Basic::New<Setting>();
			return instance;
		}
	};
}

#endif//!_UI_SETTING_SETTING_H_
