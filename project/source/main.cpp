#include "DirectX 11/window/WindowDX.h"
#include "Ray Casting/window/WindowRC.h"
#include "ApplicationRC.h"
#include "ApplicationDX.h"



int WINAPI WinMain(_In_ HINSTANCE appHandle, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR cmdLine, _In_ int windowShowParams)
{

	ApplicationDX application;

	application.Init(appHandle, windowShowParams);
	application.Run();


	HRESULT hr = engine::DX::g_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	return EXIT_SUCCESS;
}

