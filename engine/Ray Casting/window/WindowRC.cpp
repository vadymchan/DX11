#include "WindowRC.h"
#include <iostream>
#include "../dependencies/glm/glm/ext/vector_float2.hpp"

namespace engine::RC
{



	void Window::initConsole()
	{
		AllocConsole();
		FILE* dummy;

		freopen_s(&dummy, "CONIN$", "r", stdin);
		freopen_s(&dummy, "CONOUT$", "w", stdout);
		freopen_s(&dummy, "CONOUT$", "w", stderr);

	}

	void Window::initWindow(const LPCWSTR& title, int xStart, int yStart,  int width, int height, const HINSTANCE& appHandle, int windowShowParams)
	{



		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = appHandle;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIconSm = LoadIcon(NULL, IDI_SHIELD);
		wc.lpszClassName = L"Window";


		RegisterClassEx(&wc);
		hWnd = CreateWindowEx(NULL,
			L"Window",
			title,
			WS_OVERLAPPEDWINDOW,
			xStart,
			yStart,
			width,
			height,
			NULL,
			NULL,
			appHandle,
			this
		);
		ShowWindow(hWnd, windowShowParams);
	}

	void Window::initColorBuffer()
	{
		RECT ClientsRect;

		GetClientRect(hWnd, &ClientsRect);


		ClientWidth = ClientsRect.right - ClientsRect.left;
		ClientHeight = ClientsRect.bottom - ClientsRect.top;

		BitmapBufferWidth = ClientWidth / windowBufferRatio;
		BitmapBufferHeight = ClientHeight / windowBufferRatio;

		DeviceContext = GetDC(hWnd);



		BitmapMemory = VirtualAlloc(0, static_cast<SIZE_T>(BitmapBufferWidth * BitmapBufferHeight * bytePerPixel), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


		flush();

	}

	void Window::flush()
	{
		bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
		bitmapInfo.bmiHeader.biWidth = BitmapBufferWidth;
		bitmapInfo.bmiHeader.biHeight = -BitmapBufferHeight; // top left instead of bottom left
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = bytePerPixel * 8; // 4 bytes to bits
		bitmapInfo.bmiHeader.biCompression = BI_RGB;

		StretchDIBits(DeviceContext,
			0, 0,
			ClientWidth, ClientHeight,
			0, 0,
			BitmapBufferWidth, BitmapBufferHeight,
			BitmapMemory,
			&bitmapInfo,
			DIB_RGB_COLORS, SRCCOPY);
	}

	void Window::terminate()
	{
		ExitProcess(0);
	}

	void Window::drawPixel(int x, int y, uint32_t color) const
	{
		uint32_t* pixel = reinterpret_cast<uint32_t*>(BitmapMemory);
		pixel += y * BitmapBufferWidth + x;
		*pixel = color;
	}

	inline void Window::clearScreen(uint32_t color)
	{
		uint32_t* pixel = reinterpret_cast<uint32_t*>(BitmapMemory);
		for (int y = 0; y < BitmapBufferWidth; ++y)
		{
			for (int x = 0; x < BitmapBufferHeight; ++x)
			{
				*pixel++ = color;
			}
		}

	}

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		LONG_PTR voidPtr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		Window* window = reinterpret_cast<Window*>(voidPtr);
		
		static glm::vec2 windowSize;

		switch (message)
		{

		case WM_CREATE:
		{
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			//window = reinterpret_cast<Window*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
			break;
		}

		case WM_SIZE:
			windowSize = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			{
				SendMessage(hWnd, WM_EXITSIZEMOVE, wParam, lParam);
			}
			break;

		case WM_EXITSIZEMOVE:
			
			if (windowSize.x <= 1 || windowSize.y <= 1) // minimize the window
			{
				break;
			}
			window->SetWindowSize(windowSize.x, windowSize.y);

			window->SetBufferSize(windowSize.x / window->GetWindowBufferRatio(),
				windowSize.y / window->GetWindowBufferRatio());

			window->SetBitmapMemory(VirtualAlloc(0
				, static_cast<SIZE_T>(window->GetBufferWidth() * window->GetBufferHeight() * window->bytePerPixel)
				, MEM_RESERVE | MEM_COMMIT
				, PAGE_READWRITE));
			break;
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

		// Handle any messages the switch statement didn't
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

}



