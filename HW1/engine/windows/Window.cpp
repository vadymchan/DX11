#include "Window.h"


void Window::initConsole()
{
	AllocConsole();
	FILE* dummy;
	auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
														
}

void Window::initWindow(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams, const WNDPROC& WindowProc)
{

	

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = appHandle;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"Window";




	RegisterClassExW(&wc);
	hWnd = CreateWindowExW(NULL,
		L"Window",
		title,
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		500,
		400,
		NULL,
		NULL,
		appHandle,
		NULL);
	ShowWindow(hWnd, windowShowParams);
}

void Window::initColorBuffer()
{
	RECT ClientsRect;

	GetClientRect(hWnd, &ClientsRect);


	ClientWidth = ClientsRect.right - ClientsRect.left;
	ClientHeight = ClientsRect.bottom - ClientsRect.top;

	BitmapBufferWidth = ClientWidth ;
	BitmapBufferHeight = ClientHeight ;

	DeviceContext = GetDC(hWnd);

	int bytePerPixel = 4;

	BitmapMemory = VirtualAlloc(0, static_cast<SIZE_T>(BitmapBufferWidth * BitmapBufferHeight * bytePerPixel), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	
	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = BitmapBufferWidth;
	bitmapInfo.bmiHeader.biHeight = -BitmapBufferHeight; // top left instead of bottom left
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32; // 4 bytes 
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
}

void Window::flush()
{

	
	StretchDIBits(DeviceContext,
		0, 0,
		ClientWidth, ClientHeight,
		0, 0,
		BitmapBufferWidth, BitmapBufferHeight,
		BitmapMemory,
		&bitmapInfo,
		DIB_RGB_COLORS, SRCCOPY);
}


