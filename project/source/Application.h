#pragma once



#include <utils/FPSTimer.h>
#include <windows/Window.h>
#include <render/Scene.h>
#include <render/Camera.h>


class Application
{

public:
	Application() {};
	Application(float cameraSpeed, const Camera& camera, const Engine::Scene& scene) 
		: cameraSpeed(cameraSpeed)
		, camera(camera)
		, scene(scene)
	{
		initSpherePos = scene.sphere.getCenter();
	}

	Application& operator=(const Application& other)
	{
		cameraSpeed = other.cameraSpeed;
		camera = other.camera;
		scene = other.scene;
		return *this;
	}

	void WindowSetup(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams, const WNDPROC& WindowProc);
	
	void Run();
	bool ProcessInputs();

	void MoveSphere(glm::vec3 offset);
	void MoveCamera(glm::vec3 offset);
	void OnMousePressed(float xPos, float yPos);
	void OnMouseReleased();
	void Close();
	float getCameraSpeed() { return cameraSpeed; }
	bool isMousePressed() { return mousePressed; }

	Camera camera;
	FPSTimer fpsTimer;
private:

	MSG msg;
	Window window;
	Engine::Scene scene;

	glm::vec2 lastMousePosition;
	glm::vec2 mouseOffsets;
	glm::vec3 initSpherePos;
	
	float cameraSpeed;
	bool mousePressed;
	

};

