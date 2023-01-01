#pragma once
#include <windows.h>
#include <windowsx.h>

#include <cstdio>
#include <sstream>
//
//template<typename ...Args>
//void printToVisualStudioOutput(Args&&... args)
//{
//	std::stringstream ss;
//	(ss << ... << args) << std::endl; // Fold expression requires C++17
//	OutputDebugStringA(ss.str().c_str()); // not work when use OutputDebugStringW
//}


class Window
{
public:

	

	void initConsole();

	void initWindow(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams, const WNDPROC& WindowProc);

	void initColorBuffer();

	void flush();

	void drawPixel(int x, int y, uint32_t color)
	{
		uint32_t* pixel = reinterpret_cast<uint32_t*>(BitmapMemory);
		pixel += y * BitmapBufferWidth + x;
		*pixel = color;
	}


	void clearScreen(uint32_t color)
	{
		uint32_t* pixel = reinterpret_cast<uint32_t*>(BitmapMemory);
		for (int y = 0; y < BitmapBufferWidth; ++y)
		{
			for (int x = 0; x < BitmapBufferHeight; ++x)
			{
				*pixel++ = color;
			}

		}

		//alternative
		/*for (int Index = 0;
			Index < BitmapBufferWidth * BitmapBufferHeight;
			++Index) {
			*pixel++ = color;
		}*/


	}

	inline int GetBufferWidth() const { return BitmapBufferWidth; }
	inline int GetBufferHeight() const { return BitmapBufferHeight; }
	inline int GetClientWidth() const { return ClientWidth; }
	inline int GetClientHeight() const { return ClientHeight; }

private:
	void* BitmapMemory;

	int BitmapBufferWidth;
	int BitmapBufferHeight;
	int ClientWidth;
	int ClientHeight;
	int height;
	int width;

	HWND hWnd;
	WNDCLASSEX wc;
	HDC DeviceContext;
	BITMAPINFO bitmapInfo;

};

