#pragma once
#include "../window/WindowDX.h"
#include "Utils/Camera.h"
#include "MeshSystem/MeshSystem.h"
#include "Texture/Skybox.h"
#include "Texture/SampleState.h"

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

			setSampleState();

			MeshSystem::getInstance().render(camera, visualizeNormal);

			if (m_skybox.get() != nullptr)
			{
				m_skybox.get()->render(camera);
			}

			std::cout << "Current render state: " << (UINT)currentSampleState << std::endl;

			window.flush();
			window.clearDepthStencil();
		}


		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::shared_ptr<Skybox>& skybox = std::shared_ptr<Skybox>())
		{
			initRasterizerDesc(rasterizerDesc);
			createRasterizerState();

			initSamplers();

			m_skybox = skybox;
		}


		void changeRenderState(SampleState::BindSlot bindSlot)
		{
			currentSampleState = bindSlot;
			updateConstantBufferSamplerState = true;
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

		void setSkybox(const std::shared_ptr<Skybox>& skybox)
		{
			m_skybox = skybox;
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

		void setSampleState()
		{
			switch (currentSampleState)
			{
			case engine::DX::SampleState::BindSlot::POINT_WRAP:
				g_pointWrap.bind();
				break;
			case engine::DX::SampleState::BindSlot::LINEAR_WRAP:
				g_linearWrap.bind();
				break;
			case engine::DX::SampleState::BindSlot::ANISOTROPIC_WRAP:
				g_anisotropicWrap.bind();
				break;
			default:
				g_anisotropicWrap.bind();
				break;
			}

			if (updateConstantBufferSamplerState)
			{
				samplerStateIndexBuffer.setBufferData({ {(UINT)currentSampleState, 0, 0, 0 } });
				updateConstantBufferSamplerState = false;
			}

			samplerStateIndexBuffer.setPixelShaderBuffer();

		}

		void initSamplers()
		{
			samplerStateIndexBuffer.initBuffer(SELECTED_SAMPLER_STATE_3, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			samplerStateIndexBuffer.setBufferData({ {(UINT)currentSampleState, 0, 0, 0 } });

			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			samplerDesc.BorderColor[0] = 0.0f;
			samplerDesc.BorderColor[1] = 0.0f;
			samplerDesc.BorderColor[2] = 0.0f;
			samplerDesc.BorderColor[3] = 0.0f;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			g_pointWrap.initSampleState((UINT)SampleState::BindSlot::POINT_WRAP, samplerDesc);
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			g_linearWrap.initSampleState((UINT)SampleState::BindSlot::LINEAR_WRAP, samplerDesc);
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			g_anisotropicWrap.initSampleState((UINT)SampleState::BindSlot::ANISOTROPIC_WRAP, samplerDesc);
		}

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

		SampleState g_pointWrap;
		SampleState g_linearWrap;
		SampleState g_anisotropicWrap;
		struct constantBufferSamplerState
		{
			UINT currentSampleState;
			float padding1;
			float padding2;
			float padding3;
		};
		ConstantBuffer<constantBufferSamplerState> samplerStateIndexBuffer; // float4 for adding paddings
		SampleState::BindSlot currentSampleState{ SampleState::BindSlot::ANISOTROPIC_WRAP };

		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;

		std::shared_ptr<Skybox> m_skybox;


		bool visualizeNormal{};
		bool needToUpdateRasterizer{};
		bool updateConstantBufferSamplerState{};

	};

}