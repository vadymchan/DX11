#include "FPSTimerRC.h"

namespace engine::general
{



	double FPSTimer::frameTimeElapsed()
	{
		using dmil = std::chrono::duration<double, std::milli>;
		double deltaTime = std::chrono::duration_cast<dmil>(std::chrono::high_resolution_clock::now() - lastTime).count();
		return deltaTime;
	}

	void FPSTimer::resetClock()
	{
		lastTime = std::chrono::high_resolution_clock::now();
	}

}