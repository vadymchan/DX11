#pragma once

#include <chrono>

namespace engine::general
{

	class FPSTimer
	{
	public:

		double frameTimeElapsed();

		void resetClock();
		const double oneFPS = 1000 / 60;
		const double milliToSec = 1e-3;

	private:
		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

	};

}