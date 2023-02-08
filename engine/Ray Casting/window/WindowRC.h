#pragma once

#include "../../general/include/win.h"

#include <cstdio>
#include <sstream>




namespace engine::RC
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	

	class Window
	{
	public:

		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);

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
		inline const HWND& GetHWND() const { return hWnd; }
		inline void SetBitmapMemory(void* value) { VirtualFree(BitmapMemory, 0, MEM_RELEASE); BitmapMemory = value; }
		inline void SetWindowSize(int width, int height) { ClientWidth = width; ClientHeight = height; }
		inline void SetBufferSize(int width, int height) { BitmapBufferWidth = width; BitmapBufferHeight = height; }


		const int bytePerPixel{ 4 };
		const float windowBufferRatio{ 2 };
	private:
		void* BitmapMemory{};

		int BitmapBufferWidth{};
		int BitmapBufferHeight{};
		int ClientWidth{};
		int ClientHeight{};
		int height{};
		int width{};


		HWND hWnd;
		WNDCLASSEX wc;
		HDC DeviceContext;
		BITMAPINFO bitmapInfo;

	};

}