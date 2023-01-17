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


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class Window
{
public:
	void initConsole();

	void initWindow(const LPCWSTR& title, int yStart, int xStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);

	void initColorBuffer();

	void flush();

	static void terminate();

	void drawPixel(int x, int y, uint32_t color) const;

	void clearScreen(uint32_t color);

	inline int GetBufferWidth() const { return BitmapBufferWidth; }
	inline int GetBufferHeight() const { return BitmapBufferHeight; }
	inline int GetClientWidth() const { return ClientWidth; }
	inline int GetClientHeight() const { return ClientHeight; }
	inline float GetWindowBufferRatio() const { return windowBufferRatio; }
	inline void SetBitmapMemory(void* value) { VirtualFree(BitmapMemory, 0, MEM_RELEASE); BitmapMemory = value; }
	inline void SetWindowSize(int width, int height) { ClientWidth = width; ClientHeight = height; }
	inline void SetBufferSize(int width, int height) { BitmapBufferWidth = width; BitmapBufferHeight = height; }

	const int bytePerPixel{ 4 };
	const float windowBufferRatio{ 2 };
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

