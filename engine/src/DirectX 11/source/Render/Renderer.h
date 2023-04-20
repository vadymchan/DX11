#pragma once
#include "../../window/WindowDX.h"
#include "../Utils/Camera.h"
#include "../MeshSystem/MeshSystem.h"
#include "../Texture/Skybox.h"
#include "../Texture/SampleState.h"
#include "../PostProcess/HDR/PostProcess.h"

namespace engine::DX
{
	class Renderer
	{
	public:

		

		void render(Window& window, Camera& camera)
		{


			ImGuiManager::getInstance().NewFrame();
			static float offScreenBgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			updateOffScreenRenderer(window);
			updateRasterizer();

			setOffScreenRenderer();

			g_devcon->RSSetState(rasterizerState.Get());

			window.clearWindow();
			g_devcon->ClearRenderTargetView(m_offscreenRTV.Get(), offScreenBgColor);
			//window.setViews();
			
			camera.setCameraBufferVertexShader();
			
			setSampleState();

			MeshSystem::getInstance().render(camera, visualizeNormal);

			if (m_skybox.get() != nullptr)
			{
				m_skybox.get()->render(camera);
			}

			window.setViews();
			m_HDRtexture.bind();
			m_postProcess.resolve(m_HDRtexture.getShaderResourceView(), window.GetRenderTargetView());
			

			

			ImGuiManager::getInstance().Render();

			window.flush();

			
			g_devcon->ClearDepthStencilView(m_offscreenDSB.getPDepthStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0);
			window.clearDepthStencil();

		}


		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::shared_ptr<Skybox>& skybox = std::shared_ptr<Skybox>())
		{
			initRasterizerDesc(rasterizerDesc);
			createRasterizerState();

			initSamplers();
			m_postProcess.init();

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

		const PostProcess& getPostProcess() const
		{
			return m_postProcess;
		}

		PostProcess& getPostProcess()
		{
			return m_postProcess;
		}

	private:

		void updateOffScreenRenderer(Window& window)
		{
			const D3D11_TEXTURE2D_DESC& backBufferDesc = window.GetBackBufferDesc();
			const D3D11_TEXTURE2D_DESC& hdrTextureDesc = m_HDRtexture.getTextureDesc();
			if (backBufferDesc.Width != hdrTextureDesc.Width 
				|| backBufferDesc.Height != hdrTextureDesc.Height)
			{
				//texture 2d
				initHDRTexture(window.GetBackBufferDesc());

				//depth stencil view
				m_offscreenDSB = window.GetDepthStencilBuffer();

				//render target view
				g_device->CreateRenderTargetView(m_HDRtexture.getTexture2DView(), nullptr, m_offscreenRTV.ReleaseAndGetAddressOf());
			}
			
		}

		void setOffScreenRenderer()
		{
			m_offscreenDSB.setDepthStencilState();
			g_devcon->OMSetRenderTargets(1, m_offscreenRTV.GetAddressOf(), m_offscreenDSB.getPDepthStencilView());

		}




		void initHDRTexture(D3D11_TEXTURE2D_DESC backBufferTex)
		{

			backBufferTex.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
			backBufferTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = backBufferTex.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			m_HDRtexture.createTextureFromMemory(PostProcess::POST_PROCESS_SAMPLE_STATE_SLOT, backBufferTex, nullptr, srvDesc);
		}


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
		ConstantBuffer<constantBufferSamplerState> samplerStateIndexBuffer; // float3 for adding paddings
		SampleState::BindSlot currentSampleState{ SampleState::BindSlot::ANISOTROPIC_WRAP };

		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;

		Texture2D m_HDRtexture;
		ComPtr<ID3D11RenderTargetView> m_offscreenRTV;
		DepthStencilBuffer m_offscreenDSB;

		PostProcess m_postProcess;

		std::shared_ptr<Skybox> m_skybox;

		bool visualizeNormal{};
		bool needToUpdateRasterizer{};
		bool updateConstantBufferSamplerState{};

	};

}