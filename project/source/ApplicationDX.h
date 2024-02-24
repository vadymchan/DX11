#pragma once

#include <DirectX 11/window/WindowDX.h>
#include <general/utils/timer/FPSTimerRC.h>
#include <general/utils/console/Console.h>
#include <thread>
#include <vector>
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

protected:

	enum MaterialType {
		DIAMOND,
		OBSIDIAN,
		GRASS,
		PEACOCK,
		VINES,
		NORMAL_DEBUG,
		MIRROR,
		SHINY_CONDUCTOR,
		ROUGH_DIELECTRIC,
		NO_MATERIAL,
		DEFAULT_SKIN,
		DISSOLUTION_SKIN,
		MATERIAL_COUNT 
	};

	struct DissolutionAnimation
	{
		float initTime;
		float maxTime; 

		engine::DX::DissolutionInstances::ModelInstanceID modelInstanceID;
	};

public:

	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();
	void Update(float deltaTime, float currentTime);
	void UpdateParticleAnimation(float deltaTime);
	void UpdateDissolutionAnimation(float currentTime);
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

	void removeFinishedDissolutionAnimations(const float currentTime, std::vector<engine::DX::TransformSystem::ID>& newOpaqueTransformIDs);
	bool isAnimationFinished(const float currentTime, const DissolutionAnimation& dissolutionAnimation);
	void handleFinishedAnimation(const DissolutionAnimation& dissolutionAnimation, std::vector<engine::DX::TransformSystem::ID>& newOpaqueTransformIDs);
	void updateAnimationTime(const float currentTime, const DissolutionAnimation& dissolutionAnimation);

	std::array<bool, MoveDirection::COUNT> cameraMovingDirections{};
	engine::DX::float3 cameraDirection{};
	engine::DX::float3 cameraRotation{};
	engine::DX::float3 objMoveDirection{};
	engine::DX::float2 lastMousePos{};
	engine::general::FPSTimer fpsTimer;
	engine::DX::Engine engine;
	MSG msg;
	engine::DX::LightSystem::FlashLightID cameraFlashLight = -1;
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

	//opaque instances
	uint32_t iblOpaqueInstance;
	uint32_t sphereOpaqueInstance;

	//dissolution instances 
	uint32_t dissolutionInstancesID;
	std::vector<DissolutionAnimation> dissolutionAnimations;
	float dissolutionEffectDuration{}; // in milliseconds

	//materials
	std::array<std::shared_ptr<engine::DX::OpaqueInstances::Material>, MATERIAL_COUNT> opaqueMaterials;
	std::array<std::shared_ptr<engine::DX::DissolutionInstances::Material>, MATERIAL_COUNT> dissolutionMaterials;

	std::shared_ptr<engine::DX::Model> spawnModel;
};

