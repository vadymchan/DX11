#pragma once
#include "../window/WindowDX.h"
#include "Utils/Camera.h"
#include "MeshSystem/MeshSystem.h"

namespace engine::DX
{
	class Renderer
	{
	public:

		void render(Window& window, Camera& camera)
		{
			g_devcon->RSSetState(rasterizerState.Get());
			window.clearWindow();
			window.setViews();
			camera.setCameraBuffer();
			MeshSystem::getInstance().render(camera.position());
			window.flush();
			window.clearDepthStencil();
		}

		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc)
		{
			initRasterizerDesc(rasterizerDesc);
			createRasterizerState();
		}

		

	private:

		void initRasterizerDesc(const D3D11_RASTERIZER_DESC& rasterizerDesc)
		{
			rasterizationDesc = rasterizerDesc;
		}

		void createRasterizerState()
		{
			HRESULT result = g_device->CreateRasterizerState(&rasterizationDesc, rasterizerState.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Raserizer creation failed: ");
			}
		}

		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;
	};

}