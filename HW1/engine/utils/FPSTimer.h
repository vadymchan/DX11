#pragma once

#include <chrono>

class FPSTimer
{
public:

	double frameTimeElapsed();

	void resetClock();
	const double oneFPS = 1000 / 60;

private:
	std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
	
};