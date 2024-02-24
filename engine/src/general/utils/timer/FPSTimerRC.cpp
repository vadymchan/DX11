#include "FPSTimerRC.h"
#include <iostream>
namespace engine::general
{
	constexpr float FPSTimer::oneSecInMS = 1000.0f;
	constexpr float FPSTimer::oneFPS = 1000 / 60;
	constexpr float FPSTimer::milliToSec = 1e-3;
	constexpr float FPSTimer::initTick = static_cast<float>(GetTickCount64());

	float FPSTimer::frameTimeElapsed()
	{
		using dmil = std::chrono::duration<float, std::milli>;
		float deltaTime = std::chrono::duration_cast<dmil>(std::chrono::high_resolution_clock::now() - lastTime).count();
		timeCounter += deltaTime;
		return deltaTime;
	}

	void FPSTimer::resetClock()
	{
		frameCounter++;
		if (timeCounter >= oneSecInMS)
		{
			std::cout << "FPS: " << frameCounter << std::endl;
			frameCounter = 0;
			timeCounter -= oneSecInMS;
		}
		lastTime = std::chrono::high_resolution_clock::now();
	}

	

}