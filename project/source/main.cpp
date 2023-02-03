#include "Ray Casting/windows/Window.h"
#include "Application.h"

int WINAPI WinMain(_In_ HINSTANCE appHandle, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR cmdLine, _In_ int windowShowParams)
{

	Application application;
	application.Init(appHandle, windowShowParams);
	application.Run();

	return EXIT_SUCCESS;
}


