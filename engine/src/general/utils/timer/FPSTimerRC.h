#pragma once

#include <chrono>
#include <Windows.h>

namespace engine::general
{

	class FPSTimer
	{
	public:

		float frameTimeElapsed();
		static float getCurrentTick()
		{
			return static_cast<float>(GetTickCount64());
		}

		void resetClock();
		const static float oneFPS;
		const static float milliToSec;
		const static float initTick;
		const static float oneSecInMS;

	private:
		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

		int frameCounter = 0;
		float timeCounter = 0.0f;
	};

}

