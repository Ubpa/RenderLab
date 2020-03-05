#pragma once

#include <UI/Grid.h>

namespace Ubpa {
	class Setting : public Grid {
	public:
		using Grid::Grid;

	protected:
		virtual ~Setting() = default;

	public:
		static Ptr<Setting> GetInstance() {
			static auto instance = Ubpa::New<Setting>();
			return instance;
		}
	};
}
