#pragma once

#include "../../general/include/win.h"

#include "../source/D3D/D3D.h"
#include "../source/Buffer/DepthStencilBuffer.h"


namespace engine::DX
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


	class Window
	{
	public:
		void initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);
		void clearWindow();
		void clearDepthStencil() { g_devcon->ClearDepthStencilView(depthStencilBuffer.getPDepthStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0.0f); }
		void windowResize(float width, float height);
		void setBackgroundColor(float r, float g, float b, float a);
		void setVSync(bool value) { vsync = value; }
		/// <summary>
		/// sets Render Target View and Depth Stencil View to the pipeline
		/// </summary>
		void setViews(){ g_devcon->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilBuffer.getPDepthStencilView()); }
		const D3D11_VIEWPORT& GetViewport() const { return viewport; }
		float GetAspectRatio()
		{
			backBuffer.Get()->GetDesc(&backBufferDesc);
			return float(backBufferDesc.Width) / backBufferDesc.Height;
		}
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
		DepthStencilBuffer depthStencilBuffer;
		D3D11_VIEWPORT viewport;
		D3D11_TEXTURE2D_DESC backBufferDesc;
		FLOAT bgColorRGBA[4];
		float width{};
		float height{};
		bool vsync{};
	};


}