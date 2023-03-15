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
			MeshSystem::getInstance().render(camera, visualizeNormal);
			window.flush();
			window.clearDepthStencil();
		}

		

		void changeWireframe(bool wireframeEnabled)
		{
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

		void initRasterizator(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID
			, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK
			, BOOL            frontCounterClockwise = false
			, INT             depthBias = 0
			, FLOAT           depthBiasClamp = 0.0f
			, FLOAT           slopeScaledDepthBias = 0.0f
			, BOOL            depthClipEnable = true
			, BOOL            scissorEnable = false
			, BOOL            multisampleEnable = false
			, BOOL            antialiasedLineEnable = false)
		{
			initRasterizerDesc(fillMode, cullMode, frontCounterClockwise, depthBias, depthBiasClamp, slopeScaledDepthBias, depthClipEnable, scissorEnable, multisampleEnable, antialiasedLineEnable);
			createRasterizerState();
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

		void setVisualizeNormal(bool value)
		{
			visualizeNormal = value;
		}

	private:
		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		bool visualizeNormal{};
		bool needToUpdateRasterizer{};


		void initRasterizerDesc(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID
			, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK
			, BOOL            frontCounterClockwise = false
			, INT             depthBias = 0
			, FLOAT           depthBiasClamp = 0.0f
			, FLOAT           slopeScaledDepthBias = 0.0f
			, BOOL            depthClipEnable = true
			, BOOL            scissorEnable = false
			, BOOL            multisampleEnable = false
			, BOOL            antialiasedLineEnable = false)
		{
			rasterizationDesc.FillMode = fillMode;
			rasterizationDesc.CullMode = cullMode;
			rasterizationDesc.FrontCounterClockwise = frontCounterClockwise;
			rasterizationDesc.DepthBias = depthBias;
			rasterizationDesc.DepthBiasClamp = depthBiasClamp;
			rasterizationDesc.SlopeScaledDepthBias = slopeScaledDepthBias;
			rasterizationDesc.DepthClipEnable = depthClipEnable;
			rasterizationDesc.ScissorEnable = scissorEnable;
			rasterizationDesc.MultisampleEnable = multisampleEnable;
			rasterizationDesc.AntialiasedLineEnable = antialiasedLineEnable;
		}
	};

}