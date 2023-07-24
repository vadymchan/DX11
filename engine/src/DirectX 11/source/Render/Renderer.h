#pragma once
#include "../../window/WindowDX.h"
#include "../Utils/Camera.h"
#include "../MeshSystem/MeshSystem.h"
#include "../Texture/Skybox.h"
#include "../Texture/SampleState.h"
#include "../PostProcess/HDR/PostProcess.h"
#include "../ParticleSystem/ParticleSystem.h"

namespace engine::DX
{
	class Renderer : public IWindowResizeHandler
	{
	public:

		enum class BlendState
		{
			ENABLED,
			DISABLED,
			CUSTOM,
			COUNT
		};

		void render(Camera& camera);

		void initRenderer(const D3D11_RASTERIZER_DESC& rasterizerDesc
			, Window* const window
			, const std::shared_ptr<Skybox>& skybox = std::shared_ptr<Skybox>())
		{
			this->window = window;
			
			initRasterizator(rasterizerDesc);
			initBlendState();
			initDepthStencil(window->GetWidth(), window->GetHeight());
			setSkybox(skybox);

		}

		

		void setBlendState(ComPtr<ID3D11BlendState> blendState)
		{
			blendStates[(int)BlendState::CUSTOM] = blendState;
		}

		void changeRenderState(SamplerState::BindSlot bindSlot);

		void changeWireframe(bool wireframeEnabled);

		void setSkybox(const std::shared_ptr<Skybox>& skybox);

		void setParticleSystem(ParticleSystem* particleSystem);

		void updateRasterizer();

		void setVisualizeNormal(bool value);

		const PostProcess& getPostProcess() const;

		PostProcess& getPostProcess();

		void onWindowResized(int newWidth, int newHeight) override 
		{

			auto depthStencilTextureDesc = depthStencilBuffer.getTexture2D().getTextureDesc();

			if (depthStencilTextureDesc.Width != newWidth
				|| depthStencilTextureDesc.Height != newHeight)
			{
				initDepthStencil(newWidth, newHeight);
			}
		}



	private:

		void updateOffScreenRenderer();

		void setOffScreenRenderer();

		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc);

		void initHDRTexture(D3D11_TEXTURE2D_DESC backBufferTex);

		void setSampleState();

		void initSamplers();

		void initRasterizerDesc(const D3D11_RASTERIZER_DESC& rasterizerDesc);

		void initBlendState()
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

		void setBlendState(BlendState state, const std::array<float, 4>& blendFactor = { 0.0f, 0.0f, 0.0f, 0.0f }, UINT sampleMask = 0xFFFFFFFF)
		{
			g_devcon->OMSetBlendState(blendStates[(int)state].Get(), blendFactor.data(), sampleMask);
		}


		void initDepthStencil(int width, int height)
		{
			D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
			depthStencilTextureDesc.Width = width;
			depthStencilTextureDesc.Height = height;
			depthStencilTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
			depthStencilTextureDesc.MipLevels = 1;
			depthStencilTextureDesc.ArraySize = 1;
			depthStencilTextureDesc.SampleDesc.Count = 1;
			depthStencilTextureDesc.SampleDesc.Quality = 0;
			depthStencilTextureDesc.CPUAccessFlags = 0;
			depthStencilTextureDesc.MiscFlags = 0;
			depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
			shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			constexpr UINT BIND_SLOT = 0;

			engine::DX::Texture2D depthStencilTexture;
			depthStencilTexture.createTextureFromMemory(depthStencilTextureDesc, nullptr, shaderResourceViewDesc, BIND_SLOT);

			std::vector<std::pair<DepthStencilState::Type, D3D11_DEPTH_STENCIL_DESC>> depthStencilDescs;

			D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			depthStencilDesc.StencilEnable = false;

			depthStencilDescs.emplace_back(DepthStencilState::Type::DepthOnStencilOff, depthStencilDesc);

			
			depthStencilDesc.StencilEnable = true;
			depthStencilDesc.StencilReadMask = 0xFF; 
			depthStencilDesc.StencilWriteMask = 0xFF; 

			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			depthStencilDesc.BackFace = depthStencilDesc.FrontFace; 

			depthStencilDescs.emplace_back(DepthStencilState::Type::DepthOnStencilOn, depthStencilDesc);

			D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			depthStencilViewDesc.Flags = 0;

			depthStencilBuffer.initDepthStencil(depthStencilTexture, depthStencilDescs, depthStencilViewDesc);


		}

		void createRasterizerState();

		SamplerState g_pointWrap;
		SamplerState g_linearWrap;
		SamplerState g_anisotropicWrap;
		struct ConstantBufferSamplerState
		{
			UINT currentSampleState;
			float padding1;
			float padding2;
			float padding3;
		};
		ConstantBuffer<ConstantBufferSamplerState> samplerStateIndexBuffer; 
		SamplerState::BindSlot currentSampleState{ SamplerState::BindSlot::ANISOTROPIC_WRAP };

		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		ComPtr<ID3D11RasterizerState> transparentRasterizerState;

		Texture2D m_HDRtexture;
		ComPtr<ID3D11RenderTargetView> m_offscreenRTV;
		DepthStencilBuffer m_offscreenDSB;

		PostProcess m_postProcess;

		std::shared_ptr<Skybox> m_skybox;

		std::shared_ptr<ConstantBuffer<PerFrameIBL>> m_perFrameIBLbuffer;

		ParticleSystem* particleSystem;
		Window* window;

		DepthStencilBuffer depthStencilBuffer;
		std::array<ComPtr<ID3D11BlendState>, (int)BlendState::COUNT> blendStates;


		bool visualizeNormal{};
		bool needToUpdateRasterizer{};
		bool updateConstantBufferSamplerState{};

	};

}