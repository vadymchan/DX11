#include "WindowDX.h"
#include <iostream>



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace engine::DX
{


	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static Window* window;
		static DirectX::XMFLOAT2 windowSize;
		static bool wasMaximized = false;

		if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
			return true;

		switch (message)
		{
		case WM_CREATE:
			CREATESTRUCT* windowCreate;
			windowCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = static_cast<Window*>(windowCreate->lpCreateParams);
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
			break;
		case WM_SIZE:
			windowSize = { (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam) };

			if ((wParam == SIZE_MAXIMIZED || (wParam == SIZE_RESTORED && wasMaximized)))
			{
				window->windowResize(windowSize.x, windowSize.y);
				wasMaximized = (wParam == SIZE_MAXIMIZED);
			}

			break;
		case WM_EXITSIZEMOVE:
			if (windowSize.x <= 1 || windowSize.y <= 1)
			{
				break;
			}
			window->windowResize(windowSize.x, windowSize.y);
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	void Window::initWindow(const LPCSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
	{
		

		this->width = width;
		this->height = height;

		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = appHandle;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		wc.lpszClassName = "DirectX 11";

		RegisterClassEx(&wc);

		hwnd = CreateWindowEx(NULL
			, "DirectX 11"
			, title
			, WS_OVERLAPPEDWINDOW
			, xStart
			, yStart
			, width
			, height
			, NULL
			, NULL
			, appHandle
			, this
		);

		ShowWindow(hwnd, windowShowParams);

		ImGuiManager::getInstance().Init(hwnd);

		initSwapchain();
		initBackbuffer();
		initRenderTargetView();
		initViewport();
		initDepthStencil();
		initBlendState();
	}

	void Window::initBlendState()
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr = g_device->CreateBlendState(&blendDesc, blendStates[(int)BlendState::ENABLED].ReleaseAndGetAddressOf());
		
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		hr = g_device->CreateBlendState(&blendDesc, blendStates[(int)BlendState::DISABLED].ReleaseAndGetAddressOf());


		setBlendState(BlendState::DISABLED);
	}

	void Window::initDepthStencil()
	{
		backBuffer.Get()->GetDesc(&backBufferDesc);
		D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
		depthStencilTextureDesc.Width = backBufferDesc.Width;
		depthStencilTextureDesc.Height = backBufferDesc.Height;
		depthStencilTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.ArraySize = 1;
		depthStencilTextureDesc.SampleDesc.Count = 1;
		depthStencilTextureDesc.SampleDesc.Quality = 0;
		depthStencilTextureDesc.CPUAccessFlags = 0;
		depthStencilTextureDesc.MiscFlags = 0;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		depthStencilDesc.StencilEnable = false;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		


		engine::DX::Texture2D depthStencilTexture;
		constexpr UINT BIND_SLOT = 0;
		depthStencilTexture.createTextureFromMemory(depthStencilTextureDesc, nullptr, shaderResourceViewDesc, BIND_SLOT);

		depthStencilBuffer.initDepthStencil(depthStencilTexture, depthStencilDesc, depthStencilViewDesc);

	}

	void Window::initSwapchain()
	{
		DXGI_SWAP_CHAIN_DESC1 swapchainDesc;

		ZeroMemory(&swapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
		swapchainDesc.BufferCount = 2;
		swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.Flags = 0;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.Scaling = DXGI_SCALING_NONE;
		swapchainDesc.Stereo = false;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		HRESULT result = g_factory->CreateSwapChainForHwnd(g_device, hwnd, &swapchainDesc, nullptr, nullptr, swapchain.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			PrintError(result, L"Swapchain was not initialized");
		}
	}

	void Window::initBackbuffer()
	{
		if (backBuffer.Get() != nullptr)
		{
			g_devcon->OMSetRenderTargets(0, 0, 0);
			renderTargetView.Reset();
			backBuffer.Reset();
			swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

		}

		HRESULT result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Failed to initialize back buffer\n";
		}


	}

	void Window::initRenderTargetView()
	{
		bgColorRGBA[0] = 1.0f;
		bgColorRGBA[1] = 0.6f;
		bgColorRGBA[2] = 0.4f;
		bgColorRGBA[3] = 1.0f;

		g_device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.ReleaseAndGetAddressOf());
	}

	void Window::initViewport()
	{
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0;
		viewport.MaxDepth = 1.0;
	}

	void Window::clearWindow()
	{
		g_devcon->ClearRenderTargetView(renderTargetView.Get(), bgColorRGBA);
	}

	inline void Window::setBackgroundColor(float r, float g, float b, float a)
	{
		bgColorRGBA[0] = r;
		bgColorRGBA[1] = g;
		bgColorRGBA[2] = b;
		bgColorRGBA[3] = a;
	}

	void Window::windowResize(float width, float height)
	{
		this->width = width;
		this->height = height;

		initBackbuffer();
		initRenderTargetView();
		initViewport();
		backBuffer.Get()->GetDesc(&backBufferDesc);
		depthStencilBuffer.resizeDepthStencilTexture(backBufferDesc.Width, backBufferDesc.Height);

	}

	void Window::flush()
	{
		if (vsync)
		{
			swapchain->Present(1, 0);
		}
		else
		{
			swapchain->Present(0, 0);
		}
	}

}