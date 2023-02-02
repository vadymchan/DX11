#pragma once

#include "../../general/include/win.h"

#include "../render/D3D.h"


namespace engine::DX
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	class Window
	{
	public:
		void initConsole();
		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);
		void clearWindow();
		void windowResize(float width, float height);
		void setBackgroundColor(float r, float g, float b, float a);
		ID3D11RenderTargetView* const* GetppRenderTargetView() const { return renderTargetView.GetAddressOf(); } //rm
		const D3D11_VIEWPORT& GetViewport() const { return viewport; }
		const HWND& GetHWND() const { return hwnd; }
		void flush();

	private:
		void initSwapchain();
		void initBackbuffer();
		void initRenderTargetView();
		void initViewport();
		HWND hwnd;
		WNDCLASSEX wc;
		ComPtr<IDXGISwapChain1> swapchain;
		ComPtr<ID3D11Texture2D> backBuffer;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		D3D11_VIEWPORT viewport;
		FLOAT bgColorRGBA[4];
		float width{};
		float height{};
	};


}