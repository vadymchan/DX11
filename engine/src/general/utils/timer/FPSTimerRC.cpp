#include "FPSTimerRC.h"

namespace engine::general
{

	const double FPSTimer::oneFPS = 1000 / 60;
	const double FPSTimer::milliToSec = 1e-3;
	const float FPSTimer::initTick = static_cast<float>(GetTickCount64() / 1000.0);

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