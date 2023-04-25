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

		void render(Window& window, Camera& camera);

		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::shared_ptr<Skybox>& skybox = std::shared_ptr<Skybox>());

		void changeRenderState(SampleState::BindSlot bindSlot);

		void changeWireframe(bool wireframeEnabled);

		void setSkybox(const std::shared_ptr<Skybox>& skybox);

		void updateRasterizer();

		void setVisualizeNormal(bool value);

		const PostProcess& getPostProcess() const;

		PostProcess& getPostProcess();

	private:

		void updateOffScreenRenderer(Window& window);

		void setOffScreenRenderer();

		void initHDRTexture(D3D11_TEXTURE2D_DESC backBufferTex);


		void setSampleState();

		void initSamplers();

		void initRasterizerDesc(const D3D11_RASTERIZER_DESC& rasterizerDesc);

		void createRasterizerState();

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