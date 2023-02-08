#include "WindowDX.h"
#include <iostream>

namespace engine::DX
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static Window* window;
		static DirectX::XMFLOAT2 windowSize;

		switch (message)
		{
		case WM_CREATE:
			CREATESTRUCT* windowCreate;
			windowCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			window = reinterpret_cast<Window*>(windowCreate->lpCreateParams);
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
			if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
			{
				PostMessage(hWnd, WM_EXITSIZEMOVE, wParam, lParam);
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

	void Window::initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
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
		wc.lpszClassName = L"DirectX 11";

		RegisterClassEx(&wc);

		hwnd = CreateWindowEx(NULL
			, L"DirectX 11"
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


		initSwapchain();
		initBackbuffer();
		initRenderTargetView();
		initViewport();
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
			std::cerr << "Swapchain was not initialized\n";
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

		HRESULT result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
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
		g_devcon->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);
	}

	void Window::initViewport()
	{
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;

		g_devcon->RSSetViewports(1, &viewport);
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