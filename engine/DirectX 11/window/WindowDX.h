#pragma once

#include <windows.h>
#include <windowsx.h>



namespace DX::engine
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	class Window
	{
	public:


		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);
		const HWND& GetHWND() const { return hwnd; }


	private:
		HWND hwnd;
		WNDCLASSEX wc;

		float width{};
		float height{};
	};


}