#pragma once

#include <deque>
#include <iostream>

namespace Ubpa {
	class Timer
	{
	public:
		enum ENUM_STATE
		{
			ENUM_STATE_INIT,
			ENUM_STATE_RUNNING,
			ENUM_STATE_STOP,
		};

		Timer(bool start = false, size_t maxLogNum = 32);

		bool Start();
		bool Stop();

		// 返回 负数 表示错误
		double Log();

		void Reset();

		// 当前正在记录的时间在位置 0
		// 新记录的时间会排在前面
		double GetLog(size_t i) const;

		ENUM_STATE GetState() const;
		double GetWholeTime() const;
		std::ostream& operator <<(std::ostream& os);
	private:
		double GetCurTime() const;

		ENUM_STATE state;
		std::deque<double> logs;
		const size_t maxLogNum;
		double lastLogTime;
		double wholeTime;
	};
}
