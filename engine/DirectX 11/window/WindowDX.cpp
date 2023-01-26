#include "WindowDX.h"

namespace DX::engine
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{


		case WM_CHAR:
			switch (wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				return 1;
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 1;
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 1;
		}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


void Window::initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
{

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = appHandle;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = L"DirectX 11";

	RegisterClassEx(&wc);

	hwnd = CreateWindowEx(NULL
		, L"DirectX 11"
		, title
		, WS_OVERLAPPEDWINDOW
		, xStart
		, yStart
		, width
		, height
		, NULL
		, NULL
		, appHandle
		, NULL
	);

	ShowWindow(hwnd, windowShowParams);

}

}