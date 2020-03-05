#pragma once

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
