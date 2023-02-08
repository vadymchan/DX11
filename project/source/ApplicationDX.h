#pragma once

#include "DirectX 11/window/WindowDX.h"
#include "general/utils/timer/FPSTimerRC.h"
#include "DirectX 11/render/SceneDX.h"
#include "general/utils/console/Console.h"
#include <thread>
#include <DirectX 11/EngineDX.h>



class ApplicationDX
{

public:


	enum MoveDirection
	{
		FORWARD,
		BACK,
		RIGHT,
		LEFT,
		UP,
		DOWN,
		COUNT
	};


	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();
	void AddCameraDirection();
	void MoveCamera();
	void RotateCamera();
	~ApplicationDX()
	{
		engine::DX::D3D::Deinit();
	}

private:
	std::array<bool, MoveDirection::COUNT> cameraMovingDirections{};
	DirectX::XMVECTOR cameraDirection{};
	DirectX::XMFLOAT3 cameraRotation{};


	engine::general::FPSTimer fpsTimer;

	//need to delete
	engine::DX::Scene scene;
	engine::DX::Window window;
	//


	engine::DX::Engine engine;
	MSG msg;
	float cameraMoveCoef{};
	bool pitchYawRotation{};

};

