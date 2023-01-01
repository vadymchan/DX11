#include "windows/Window.h"
#include "Application.h"





Application application = Application();
glm::vec3 worldUp{ glm::vec3(0, 1, 0) };

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);


int WINAPI WinMain(_In_ HINSTANCE appHandle, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR cmdLine, _In_ int windowShowParams)
{

	const int ScreenWidth{ 640 };
	const int ScreenHeight{ 480 };
	const float cameraSpeed{ 0.01f };

	Engine::Sphere sphere = Engine::Sphere(glm::vec3(0, 1, 5), 3);
	

	glm::vec3 cameraPosition{ glm::vec3(0, 20, -30) };
	glm::vec3 cameraForward{ glm::vec3(0, -0.5, 1) }; 
	//glm::vec3 cameraTarget{cameraPosition - sphere.getCenter()};
	


	Engine::Light light = Engine::Light(glm::vec3(-1, -1, 0));
	Engine::Plane plane = Engine::Plane(worldUp, glm::vec3(0, -2, 0));

	Camera camera = Camera(cameraPosition, cameraForward, worldUp, 1.0f, 100.0f, 45, static_cast<float>(ScreenWidth) / ScreenHeight);

	application = Application(cameraSpeed, camera, Engine::Scene(sphere, light, plane));
	application.WindowSetup(L"Ray caster", 300, 300, ScreenWidth, ScreenHeight, appHandle, windowShowParams, WindowProc);
	application.Run();

	return EXIT_SUCCESS;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	float xPos = static_cast<float>(GET_X_LPARAM(lParam));
	float yPos = static_cast<float>(GET_Y_LPARAM(lParam));

	switch (message)
	{
	case WM_RBUTTONDOWN:

		application.OnMousePressed(xPos, yPos);
		break;
	case WM_RBUTTONUP:
		application.OnMouseReleased();
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x57: // W
			//forward
			//application.MoveCamera(-application.camera.getDirection() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			//camera move up
			//application.MoveCamera(application.camera.getCameraUp() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			
			//sphere move up in world up direction
			//application.MoveSphere(worldUp * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));

			//sphere move up in camera up direction (paralel to camera)
			application.MoveSphere(application.camera.getCameraUp() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));

			break;
		case 0x41: // A
			//application.MoveCamera(-application.camera.getCameraRight() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			
			//sphere move left
			application.MoveSphere(-application.camera.getCameraRight() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			break;
		case 0x53: // S
			// back (this is for right handed)
			//application.MoveCamera(application.camera.getDirection() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			// down
			//application.MoveCamera(-application.camera.getCameraUp() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			
			//sphere move down in world up direction
			//application.MoveSphere(-worldUp * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));

			//sphere move down in camera down direction (paralel to camera)
			application.MoveSphere(-application.camera.getCameraUp() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));

			break;
		case 0x44: // D
			//application.MoveCamera(application.camera.getCameraRight() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			
			//sphere move right 
			application.MoveSphere(application.camera.getCameraRight() * application.getCameraSpeed() * static_cast<float>(application.fpsTimer.frameTimeElapsed()));
			
			break;
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			application.Close();
			return 1;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		application.Close();
		return 1;
		break;
	case WM_QUIT:
		PostQuitMessage(0);
		application.Close();
		return 1;



	}

	if (application.isMousePressed())
	{
		application.OnMousePressed(xPos, yPos);
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

