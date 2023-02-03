#pragma once

#include "DirectX 11/window/WindowDX.h"
#include "general/utils/timer/FPSTimerRC.h"
#include "DirectX 11/render/SceneDX.h"
#include "general/utils/console/Console.h"
#include <thread>

class ApplicationDX
{

public:
	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();
	~ApplicationDX()
	{
		engine::DX::D3D::Deinit();
	}

private:
	engine::general::FPSTimer fpsTimer;
	engine::DX::Scene scene;
	engine::DX::Window window;
};

