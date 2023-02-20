#pragma once

#include <chrono>

namespace engine::general
{

	class FPSTimer
	{
	public:

		double frameTimeElapsed();

		void resetClock();
		const static double oneFPS;
		const static double milliToSec;
		
	private:
		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

	};

}

