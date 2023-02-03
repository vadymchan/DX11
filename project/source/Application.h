#pragma once



#include <Ray Casting/utils/timer/FPSTimer.h>
#include <Ray Casting/windows/Window.h>
#include <Ray Casting/render/Scene.h>
#include <Ray Casting/render/Camera.h>


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

	std::shared_ptr<engine::Camera> camera;
	FPSTimer fpsTimer;
private:

	MSG msg;
	Window window;
	std::shared_ptr<engine::scene::Scene> scene;

	glm::vec2 lastMousePosition;
	glm::vec2 mouseOffsets;
	
	float cameraSpeed;
	bool mousePressed;
	

};

