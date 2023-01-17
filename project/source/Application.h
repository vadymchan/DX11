#pragma once



#include <utils/timer/FPSTimer.h>
#include <windows/Window.h>
#include <render/Scene.h>
#include <render/Camera.h>
#include <string>
#include <thread>
#include <iostream>


class Application
{

public:
	Application() 
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

	FPSTimer fpsTimer;
private:

	MSG msg;
	Window window;
	std::shared_ptr<Engine::Camera> camera;
	std::shared_ptr<Engine::Scene> scene;

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

