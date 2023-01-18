#include "Application.h"


#include <string>
#include <thread>
#include <iostream>



void Application::WindowSetup(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
{

#ifdef _DEBUG
	window.initConsole();
#endif
	window.initWindow(title, yStart, xStart, width, height, appHandle, windowShowParams);
	window.initColorBuffer();

}

void Application::Init(const HINSTANCE& appHandle, int windowShowParams)
{
	const int ScreenWidth{ 640 };
	const int ScreenHeight{ 480 };

	cameraSpeed = 0.01f;
	glm::vec3 cameraPosition{ glm::vec3(0, 20, -30) };
	glm::vec3 cameraForward{ glm::vec3(0, -0.5, 1) };
	glm::vec3 worldUp{ glm::vec3(0, 1, 0) };


	//planes
	//----------------------------------------------------------------------------------
	engine::Material planeMaterial = engine::Material({ 0.0, 0.05, 0.0 }, { 0.4, 0.5, 0.4 }, { 0.04, 0.7, 0.04 }, .078125);
	auto mathPlane = engine::math::Plane(worldUp, glm::vec3(0, -2, 0));
	std::vector<engine::ColoredPlane> planes
	{
		engine::ColoredPlane(planeMaterial, mathPlane)
	};
	//----------------------------------------------------------------------------------


	//spheres
	//----------------------------------------------------------------------------------
	engine::Material sphereMaterial = engine::Material({ 0.1745, 0.01175, 0.01175 }, { 0.61424,	0.04136	,0.04136 }, { 0.727811,	0.626959, 0.626959 }, 0.6);
	auto mathSphere = engine::math::Sphere(glm::vec3(0, 10, 5), 10);
	//----------------------------------------------------------------------------------


	//lights
	//----------------------------------------------------------------------------------
	//direction light
	glm::vec3 lightDirection1{ glm::vec3(1, -1, 0), };
	glm::vec3 lightDirection2{ glm::vec3(-1, -1, 0), };
	glm::vec3 lightColor1{ glm::vec3(0.5, 0.5, 0.5) };
	glm::vec3 lightColor2{ glm::vec3(0.5, 0.5, 0.5) };
	//----------------------------------------------------------------------------------


	//scene
	//----------------------------------------------------------------------------------
	scene = std::make_shared<engine::scene::Scene>(engine::scene::Scene());
	scene->addSphere(mathSphere, sphereMaterial);
	scene->addPlane(mathPlane, planeMaterial);
	scene->addDirectionalLight(lightDirection1, lightColor1);
	scene->addDirectionalLight(lightDirection2, lightColor2);

	//----------------------------------------------------------------------------------


	//camera
	//----------------------------------------------------------------------------------
	camera = std::make_shared<engine::Camera>(
		engine::Camera(cameraPosition, cameraForward, worldUp, 1.0f, 100.0f, 45, static_cast<float>(ScreenWidth) / ScreenHeight));
	//----------------------------------------------------------------------------------


	//window 
	//----------------------------------------------------------------------------------
	WindowSetup(L"Ray caster", 300, 300, ScreenWidth, ScreenHeight, appHandle, windowShowParams);
	//----------------------------------------------------------------------------------

}

void Application::Run()
{

	while (true)
	{
		double timeElapsed = fpsTimer.frameTimeElapsed();

		if (timeElapsed > fpsTimer.oneFPS) // enter the scope not more than 60 times per second
		{
			if (!ProcessInputs())
			{
				return;
			}
			scene->render(window, camera);
			window.flush();
			fpsTimer.resetClock();
		}
#ifdef _DEBUG
		std::cout << "Camera\t" << "x: " << camera->getPosition().x << " y: " << camera->getPosition().y << " z: " << camera->getPosition().z << std::endl;
		std::cout << "Look at\t" << "x: " << camera->getDirection().x << " y: " << camera->getDirection().y << " z: " << camera->getDirection().z << std::endl << std::endl;

		std::cout << timeElapsed << std::endl;
#endif 

		std::this_thread::yield();
	}
}

bool Application::ProcessInputs()
{
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{

		float xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
		float yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
		switch (msg.message)
		{
		case WM_RBUTTONDOWN:
			OnMousePressed(xPos, yPos);
			break;
		case WM_RBUTTONUP:
			OnMouseReleased();
			break;
		case WM_KEYDOWN:
			switch (msg.wParam)
			{
			case 0x57: // W
				MoveCamera(camera->getWorldUp() * getCameraSpeed() * static_cast<float>(fpsTimer.frameTimeElapsed()));
				break;
			case 0x41: // A
				MoveCamera(-camera->getCameraRight() * getCameraSpeed() * static_cast<float>(fpsTimer.frameTimeElapsed()));
				break;
			case 0x53: // S
				MoveCamera(-camera->getWorldUp() * getCameraSpeed() * static_cast<float>(fpsTimer.frameTimeElapsed()));
				break;
			case 0x44: // D
				MoveCamera(camera->getCameraRight() * getCameraSpeed() * static_cast<float>(fpsTimer.frameTimeElapsed()));
				break;
			}
			break;
		case WM_QUIT:
			return false;
		}
		if (isMousePressed())
		{
			OnMousePressed(xPos, yPos);
		}
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
		//check for change window size
		if ((float)window.GetClientWidth() / window.GetClientHeight() != camera->getAspectRatio())
		{
			camera->changeAspectRatio((float)window.GetClientWidth() / window.GetClientHeight());
		}

	}
	return true;


}


void Application::MoveCamera(glm::vec3 offset)
{
	camera->changePosition(camera->getPosition() + offset);
}

void Application::OnMousePressed(float xPos, float yPos)
{
	if (!mousePressed)
	{
		lastMousePosition = glm::vec2(xPos, yPos);
		mousePressed = true;
		//initSpherePos = scene.sphere.getCenter();
		return;
	}
	float xOffset{ xPos - lastMousePosition.x };
	float yOffset{ yPos - lastMousePosition.y };

#ifdef _DEBUG
	std::cout << "lastPostion\t" << "x: " << lastMousePosition.x << " y: " << lastMousePosition.y << std::endl;
	std::cout << "Offsets\t" << "x: " << xOffset << " y: " << yOffset << std::endl << std::endl;
#endif // DEBUG

	lastMousePosition = glm::vec2(xPos, yPos);
	camera->changePosition(camera->getPosition() + glm::vec3(xOffset, yOffset, 0));
}

void Application::OnMouseReleased()
{
	mousePressed = false;
	lastMousePosition = glm::vec2(window.GetClientWidth() / 2, window.GetClientHeight() / 2);
}

void Application::Close()
{
	ExitProcess(0);
}
