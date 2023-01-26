#pragma once

#include "DirectX 11/window/WindowDX.h"
#include "general/utils/timer/FPSTimerRC.h"
#include "DirectX 11/render/SceneDX.h"
#include <thread>

class ApplicationDX
{

public:
	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();
	~ApplicationDX()
	{
		DX::engine::D3D::Deinit();
	}

private:
	general::engine::FPSTimer fpsTimer;
	DX::engine::Scene scene;
	DX::engine::Window window;
};

