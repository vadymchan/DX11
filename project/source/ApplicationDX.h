#pragma once

#include <DirectX 11/window/WindowDX.h>
#include <general/utils/timer/FPSTimerRC.h>
#include <general/utils/console/Console.h>
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

	enum class ChangeState
	{
        NONE = -1,
		INCREASE,
		DECREASE,
		COUNT
	};

	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();
	void AddCameraDirection();
	bool MoveCamera();
	void MoveObject(float xPos, float yPos);
	void RotateCamera(float xPos, float yPos);
	void HandleExposure();
	std::vector<engine::DX::float4x4> generateRandomInstances(size_t numInstances);
	~ApplicationDX()
	{
		engine::DX::D3D::Deinit();
	}

private:
	std::array<bool, MoveDirection::COUNT> cameraMovingDirections{};
	engine::DX::float3 cameraDirection{};
	engine::DX::float3 cameraRotation{};
	engine::DX::float3 objMoveDirection{};
	engine::DX::float2 lastMousePos{};
	engine::general::FPSTimer fpsTimer;
	engine::DX::Engine engine;
	MSG msg;
	engine::DX::LightSystem::FlashLightID cameraFlashLight;
	float cameraSpeed{};
	float cameraRotationSpeed{};
	float cameraMaxPitch{};
	float cameraMinPitch{};
	float cameraPitch{};
	float deltaTime{};
	bool pitchYawRotation{};
	bool increasedSpeed{};
	bool rayCasted{};
	bool objectCaptured{};
	bool nomalVisualized{};
	bool wireframeMode{};
	bool flashLightAttached{};
	std::array<bool, int(COUNT)> exposureState{};
	float deltaExposure{};
};

