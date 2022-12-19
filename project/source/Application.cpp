#include "Application.h"


#include <string>
#include <thread>
#include <iostream>


void Application::WindowSetup(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams, const WNDPROC& WindowProc)
{



#ifdef _DEBUG
#endif
	window.initConsole();
	window.initWindow(title, yStart, xStart, width, height, appHandle, windowShowParams, WindowProc);
	window.initColorBuffer();

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


			scene.render(window, camera);
			window.flush();

#ifdef DEBUG
			/*std::cout << "Camera\t" << "x: " << camera.getPosition().x << " y: " << camera.getPosition().y << " z: " << camera.getPosition().z << std::endl;
			std::cout << "Look at\t" << "x: " << camera.getDirection().x << " y: " << camera.getDirection().y << " z: " << camera.getDirection().z << std::endl<< std::endl;
			*/

			std::cout << "Sphere\t" << "x: " << scene.sphere.getCenter().x << " y: " << scene.sphere.getCenter().y << " z: " << scene.sphere.getCenter().z << std::endl << std::endl;

#endif // DEBUG






			fpsTimer.resetClock();
		}
#ifdef DEBUG
		std::cout << timeElapsed << std::endl;

#endif // fps Timer


		std::this_thread::yield();
	}
}

bool Application::ProcessInputs()
{
	while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{

		TranslateMessage(&msg);
		LRESULT callbackResult = DispatchMessageW(&msg);
		if (callbackResult == 1)
		{
			return false;
		}
	}
	return true;


}

void Application::MoveSphere(glm::vec3 offset)
{
	scene.sphere.setCenter((scene.sphere.getCenter() + offset));

}

void Application::MoveCamera(glm::vec3 offset)
{
	camera.changePosition(camera.getPosition() + offset);

}

//TODO:adjust sphere speed during mouse movement
void Application::OnMousePressed(float xPos, float yPos)
{
	if (!mousePressed)
	{
		lastMousePosition = glm::vec2(xPos, yPos);
		mousePressed = true;
		initSpherePos = scene.sphere.getCenter();
		return;
	}
	float xOffset{ xPos - lastMousePosition.x };
	float yOffset{ yPos - lastMousePosition.y };

#ifdef DEBUG
	std::cout << "lastPostion\t" << "x: " << lastMousePosition.x << " y: " << lastMousePosition.y << std::endl;
	std::cout << "Offsets\t" << "x: " << xOffset << " y: " << yOffset << std::endl << std::endl;
#endif // DEBUG

	scene.sphere.setCenter(
		scene.sphere.getCenter() + glm::vec3(xOffset, yOffset, 0) * getCameraSpeed());


	lastMousePosition = glm::vec2(xPos, yPos);

	//TODO:: use clip space cooridinates instead of screen space
	//float halfImageHeight{  (std::tan(glm::radians(camera.getFOV() / 2)) * camera.getNearPlane()) };
	//float imageHeight{ halfImageHeight * 2 };
	//float imageScreenCoeff{ imageHeight / window.GetClientWidth() 
	//auto x{ imageScreenCoeff * 1 / camera.getAspectRatio() * xOffset };
	//auto y{ imageScreenCoeff * yOffset };
	//float deltaTime{ static_cast<float>(fpsTimer.frameTimeElapsed()) };
	//scene.sphere.setCenter(
	//	glm::vec3(scene.sphere.getCenter().x + x,
	//		scene.sphere.getCenter().y + y,
	//		scene.sphere.getCenter().z));




	//camera.changePosition(camera.getPosition() + glm::vec3(xOffset, 0, yOffset));
}

void Application::OnMouseReleased()
{
	mousePressed = false;
	lastMousePosition = glm::vec2(window.GetClientWidth() / 2, window.GetClientHeight() / 2);
	scene.sphere.setCenter(initSpherePos);

}

void Application::Close()
{
	ExitProcess(0);
}
