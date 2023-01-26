#pragma once



#include <general/utils/timer/FPSTimerRC.h>
#include <general/Camera.h>
#include <Ray Casting/render/SceneRC.h>
#include <Ray Casting/window/WindowRC.h>
#include <string>
#include <thread>
#include <iostream>


class ApplicationRC
{

public:
	ApplicationRC() 
	{
		msg = MSG();
		cameraSpeed = 0;
		lastMousePosition = glm::vec2(-1);
		mouseOffsets = glm::vec2(0);
		pitchYawRotation = false;
	}


	void WindowSetup(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);
	
	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();

	void DragObject(float x, float y, float deltaTime);
	void MoveCamera(float deltaTime);
	void OnMove(const glm::vec3& dir);
	void EndMove();
	void RotateCamera(float deltaTime);
	void RollRotate(float roll);
	void PitchYawRotate(float x, float y);
	void EndRotate();
	void OnShiftPressed();
	void OnShiftReleased();
	void Close();

	general::engine::FPSTimer fpsTimer;
private:

	MSG msg;
	RC::engine::Window window;
	std::shared_ptr<general::engine::Camera> camera;
	std::shared_ptr<RC::engine::Scene> scene;

	glm::vec2 lastMousePosition{};
	glm::vec2 initMousePosition{};
	glm::vec2 mouseOffsets{};
	
	
	glm::vec3 worldUp{};
	glm::vec3 cameraDir{};
	glm::vec3 cameraAngles{};
	float cameraSpeed{};
	float rotationSpeed{};
	float cameraAspectRatio{};
	float cameraNearZ{};
	float cameraFarZ{};
	float cameraFov{};
	float rollRotationCoeff{};
	float nearPlaneHeight{};
	float nearPlaneWidth{};
	float dragCoef{};
	bool cameraMove{};
	bool rollRotation{};
	bool pitchYawRotation{};
	bool increasingSpeed{};
	bool captureObj{};
	

};

