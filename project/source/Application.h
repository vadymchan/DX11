#pragma once



#include <utils/timer/FPSTimer.h>
#include <windows/Window.h>
#include <render/Scene.h>
#include <render/Camera.h>


class Application
{

public:
	Application() 
	{
		msg = MSG();
		cameraSpeed = 0;
		lastMousePosition = glm::vec2(0);
		mouseOffsets = glm::vec2(0);
		mousePressed = false;
	}



	void WindowSetup(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);
	
	void Init(const HINSTANCE& appHandle, int windowShowParams);
	void Run();
	bool ProcessInputs();

	void MoveCamera(glm::vec3 offset);
	void OnMousePressed(float xPos, float yPos);
	void OnMouseReleased();
	void Close();
	float getCameraSpeed() { return cameraSpeed; }
	bool isMousePressed() { return mousePressed; }

	std::shared_ptr<Engine::Camera> camera;
	FPSTimer fpsTimer;
private:

	MSG msg;
	Window window;
	std::shared_ptr<Engine::Scene> scene;

	glm::vec2 lastMousePosition;
	glm::vec2 mouseOffsets;
	
	float cameraSpeed;
	bool mousePressed;
	

};

