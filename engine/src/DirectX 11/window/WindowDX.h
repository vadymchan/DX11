#pragma once

#include <array>

#include "../../general/include/win.h"
#include "../source/D3D/D3D.h"
#include "../source/Buffer/DepthStencilBuffer.h"
#include "../source/Controller/ImGuiManager.h"


namespace engine::DX
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	class IWindowResizeHandler 
	{
	public:
		virtual void onWindowResized(int newWidth, int newHeight) = 0;
	};

	class Window
	{
	public:

		/*enum class BlendState
		{
			ENABLED,
			DISABLED,
			CUSTOM,
			COUNT
		};*/

		void initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams);
		void clearWindow();
		void clearDepthStencil() { g_devcon->ClearDepthStencilView(depthStencilBuffer.getPDepthStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0.0f); }
	
		/*void setBlendState(BlendState state, const std::array<float, 4>& blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f }, UINT sampleMask = 0xFFFFFFFF)
		{
			g_devcon->OMSetBlendState(blendStates[(int)state].Get(), blendFactor.data(), sampleMask);
		}

		void setBlendState(ComPtr<ID3D11BlendState> blendState)
		{
			blendStates[(int)BlendState::CUSTOM] = blendState;
		}*/

		void resize(float width, float height);

		void registerResizeHandler(IWindowResizeHandler* handler) 
		{
			resizeHandlers.push_back(handler);
		}


		void setBackgroundColor(float r, float g, float b, float a);
		void setVSync(bool value) { vsync = value; }
		/// <summary>
		/// sets Render Target View and Depth Stencil View to the pipeline
		/// </summary>
		//void setViews(){ g_devcon->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilBuffer.getPDepthStencilView()); }
		const D3D11_VIEWPORT& GetViewport() const { return viewport; }
		void SetViewport() { g_devcon->RSSetViewports(1, &viewport); }
		//void SetDepthStencilState() { depthStencilBuffer.setDepthStencilState(); }
		float GetAspectRatio()
		{
			auto backBufferDesc = backBuffer.getTextureDesc();
			return float(backBufferDesc.Width) / backBufferDesc.Height;
		}
		const D3D11_TEXTURE2D_DESC& GetBackBufferDesc() {  return backBuffer.getTextureDesc(); }
		const HWND& GetHWND() const { return hwnd; }
		const DepthStencilBuffer& GetDepthStencilBuffer() const { return depthStencilBuffer; }
		UINT GetWidth() const { return backBuffer.getTextureDesc().Width;}
		UINT GetHeight() const { return backBuffer.getTextureDesc().Height; }
		ID3D11RenderTargetView* const GetRenderTargetViewPointer() const { return renderTargetView.Get(); }
		ID3D11RenderTargetView* const* GetRenderTargetViewAddress() const { return renderTargetView.GetAddressOf(); }
		//bool isDepthtStencilUpdated() const { return depthStencilUpdated; }
		void flush();
	private:
		

		void initSwapchain();
		void initBackbuffer();
		void initRenderTargetView();
		void initViewport();
		//void initDepthStencil();
		//void initBlendState();

		std::vector<IWindowResizeHandler*> resizeHandlers;
		void informResizeHandlers() 
		{
			for (IWindowResizeHandler* handler : resizeHandlers) 
			{
				handler->onWindowResized(width, height);
			}
		}

		HWND hwnd;
		WNDCLASSEX wc;
		ComPtr<IDXGISwapChain1> swapchain;
		Texture2D backBuffer;
		ComPtr<ID3D11RenderTargetView> renderTargetView;
		//std::array<ComPtr<ID3D11BlendState>, (int)BlendState::COUNT> blendStates;
		DepthStencilBuffer depthStencilBuffer;
		D3D11_VIEWPORT viewport;
		FLOAT bgColorRGBA[4];
		float width{};
		float height{};
		bool vsync{};
		//bool depthStencilUpdated{};	
	};


}

