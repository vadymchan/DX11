#pragma once

#include <chrono>
#include <Windows.h>

namespace engine::general
{

	class FPSTimer
	{
	public:

		double frameTimeElapsed();
		static float getCurrentTick()
		{
			return static_cast<float>(GetTickCount64() / 1000.0);
		}

		void resetClock();
		const static double oneFPS;
		const static double milliToSec;
		const static float initTick;

	private:
		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

	};

}

