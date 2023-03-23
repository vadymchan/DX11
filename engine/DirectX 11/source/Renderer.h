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
			updateRasterizer();
			g_devcon->RSSetState(rasterizerState.Get());
			window.clearWindow();
			window.setViews();
			camera.setCameraBufferVertexShader();
			TextureManager::getInstance().getSamplerState(L"modelSampler").bind();
			MeshSystem::getInstance().render(camera, visualizeNormal);
			window.flush();
			window.clearDepthStencil();
		}

		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc)
		{
			initRasterizerDesc(rasterizerDesc);
			createRasterizerState();

			TextureManager::getInstance().addSamplerState(L"modelSampler", 0, {});
		}


		void changeWireframe(bool wireframeEnabled)
		{
			initRasterizerDesc(rasterizationDesc);
			createRasterizerState();
			if (wireframeEnabled)
			{
				rasterizationDesc.FillMode = D3D11_FILL_WIREFRAME;
			}
			else
			{
				rasterizationDesc.FillMode = D3D11_FILL_SOLID;
			}
			needToUpdateRasterizer = true;
		}

		void updateRasterizer()
		{
			if (needToUpdateRasterizer)
			{
				createRasterizerState();
			}
		}

		void setVisualizeNormal(bool value)
		{
			visualizeNormal = value;
			needToUpdateRasterizer = true;
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
			needToUpdateRasterizer = false;
		}



		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		bool visualizeNormal{};
		bool needToUpdateRasterizer{};


	};

}