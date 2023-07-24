#include "Renderer.h"
#include "../Texture/ReflectionCapture.h"

namespace engine::DX
{

	void Renderer::render(Camera& camera)
	{
		
		ImGuiManager::getInstance().NewFrame();

		updateOffScreenRenderer(/*window*/);

		updateRasterizer();

		depthStencilBuffer.setDepthStencilState(DepthStencilState::Type::DepthOnStencilOff);
		//window->SetDepthStencilState();

		//clearing renderTargetView
		static float offScreenBgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		window->clearWindow();
		g_devcon->ClearRenderTargetView(m_offscreenRTV.Get(), offScreenBgColor);
		//window->setViews();

		//window->setBlendState(Window::BlendState::ENABLED);
		setBlendState(BlendState::ENABLED);

		//set global constant buffers
		LightSystem& lightSystem = LightSystem::getInstance();
		lightSystem.setBuffer();

		//texture sampler
		setSampleState();

		MeshSystem& meshSystem = MeshSystem::getInstance();
		//render shadow maps
		
		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = SHADOW_MAP_SIZE;
		viewport.Height = SHADOW_MAP_SIZE;
		viewport.MinDepth = 0.0;
		viewport.MaxDepth = 1.0;

		g_devcon->RSSetViewports(1, &viewport);

		g_devcon->RSSetState(transparentRasterizerState.Get());

		//directional light
		const int directionalLights = lightSystem.getCurrentDirectionalLights();
		for (size_t i = 0; i < directionalLights; ++i)
		{

			lightSystem.setPerViewDirectionalLight(i);
			meshSystem.renderDepth2D<MeshSystem::DepthRender::DISSOLUTION_INSTANCE>();
		}
		
		//spot light
		const int spotLights = lightSystem.getCurrentSpotLights();
		for (size_t i = 0; i < spotLights; ++i)
		{
			lightSystem.setPerViewSpotLight(i);
			meshSystem.renderDepth2D<MeshSystem::DepthRender::DISSOLUTION_INSTANCE>();
		}

		//flash light
		const int flashLights = lightSystem.getCurrentFlashLights();
		for (size_t i = 0; i < flashLights; ++i)
		{
			lightSystem.setPerViewFlashLight(i);
			meshSystem.renderDepth2D<MeshSystem::DepthRender::DISSOLUTION_INSTANCE>();
		}

		//point light
		const int pointLights = lightSystem.getCurrentPointLights();
		for (size_t i = 0; i < pointLights; ++i)
		{
			lightSystem.setPerViewPointLight(i);
			meshSystem.renderDepthCubemap<MeshSystem::DepthRender::DISSOLUTION_INSTANCE>();
		}

		
		g_devcon->RSSetState(rasterizerState.Get());
		

		//directional light
		for (size_t i = 0; i < directionalLights; ++i)
		{

			lightSystem.setPerViewDirectionalLight(i);
			meshSystem.renderDepth2D<MeshSystem::DepthRender::OPAQUE_INSTANCE>();
		}
		
		//spot light
		for (size_t i = 0; i < spotLights; ++i)
		{
			lightSystem.setPerViewSpotLight(i);
			meshSystem.renderDepth2D<MeshSystem::DepthRender::OPAQUE_INSTANCE>();
		}

		//flash light
		for (size_t i = 0; i < flashLights; ++i)
		{
			lightSystem.setPerViewFlashLight(i);
			meshSystem.renderDepth2D<MeshSystem::DepthRender::OPAQUE_INSTANCE>();
		}

		//point light
		for (size_t i = 0; i < pointLights; ++i)
		{
			lightSystem.setPerViewPointLight(i);
			meshSystem.renderDepthCubemap<MeshSystem::DepthRender::OPAQUE_INSTANCE>();
		}


		window->SetViewport();
		
		setOffScreenRenderer();


		camera.setCameraBufferVertexShader();

		meshSystem.setShowNormal(visualizeNormal);
		meshSystem.setPerFrameIBLbuffer(m_perFrameIBLbuffer);
		

		meshSystem.render<MeshSystem::RenderInstance::OPAQUE_INSTANCE>(camera);

		g_devcon->RSSetState(transparentRasterizerState.Get());

		meshSystem.render<MeshSystem::RenderInstance::DISSOLUTION_INSTANCE>(camera);
		

		meshSystem.renderUIPerFrameIBL();
		if (m_skybox.get() != nullptr)
		{
			m_skybox.get()->render(camera);
		}

		particleSystem->render(camera);


		g_devcon->OMSetRenderTargets(1, window->GetRenderTargetViewAddress(), depthStencilBuffer.getPDepthStencilView());

		m_HDRtexture.bind();
		m_postProcess.resolve(m_HDRtexture.getShaderResourceView(), window->GetRenderTargetViewPointer());


		
		ImGuiManager::getInstance().Render();


		window->flush();

		//clear depth
		g_devcon->ClearDepthStencilView(m_offscreenDSB.getPDepthStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0);
		//window->clearDepthStencil();
		g_devcon->ClearDepthStencilView(depthStencilBuffer.getPDepthStencilView(), D3D11_CLEAR_DEPTH, 0.0f, 0.0f);
		
		lightSystem.clearShadwoMaps();

	}

	void Renderer::initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc)
	{
		initRasterizerDesc(rasterizerDesc);
		createRasterizerState();

		initSamplers();
		m_postProcess.init();


		m_perFrameIBLbuffer = std::make_shared<ConstantBuffer<PerFrameIBL>>();
		m_perFrameIBLbuffer->initBuffer(PER_FRAME_IBL_BIND_SLOT, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		m_perFrameIBLbuffer->setBufferData({ {} });
		m_perFrameIBLbuffer->createBuffer();
	}

	void Renderer::changeRenderState(SamplerState::BindSlot bindSlot)
	{
		currentSampleState = bindSlot;
		updateConstantBufferSamplerState = true;
	}

	void Renderer::changeWireframe(bool wireframeEnabled)
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

	void Renderer::setSkybox(const std::shared_ptr<Skybox>& skybox)
	{
		m_skybox = skybox;
		MeshSystem::getInstance().setSkybox(skybox);
	}

	void Renderer::setParticleSystem(ParticleSystem* particleSystem)
	{
		this->particleSystem = particleSystem;

	}

	void Renderer::updateRasterizer()
	{
		if (needToUpdateRasterizer)
		{
			createRasterizerState();
		}
	}

	void Renderer::setVisualizeNormal(bool value)
	{
		visualizeNormal = value;
		needToUpdateRasterizer = true;
	}

	const PostProcess& Renderer::getPostProcess() const
	{
		return m_postProcess;
	}

	PostProcess& Renderer::getPostProcess()
	{
		return m_postProcess;
	}

	void Renderer::updateOffScreenRenderer()
	{
		const D3D11_TEXTURE2D_DESC& backBufferDesc = window->GetBackBufferDesc();
		const D3D11_TEXTURE2D_DESC& hdrTextureDesc = m_HDRtexture.getTextureDesc();
		if (backBufferDesc.Width != hdrTextureDesc.Width
			|| backBufferDesc.Height != hdrTextureDesc.Height)
		{
			//texture 2d
			initHDRTexture(window->GetBackBufferDesc());

			//depth stencil view
			//m_offscreenDSB = window->GetDepthStencilBuffer();
			m_offscreenDSB = depthStencilBuffer;

			//render target view
			g_device->CreateRenderTargetView(m_HDRtexture.getTexture2DView(), nullptr, m_offscreenRTV.ReleaseAndGetAddressOf());
		}

	}

	void Renderer::setOffScreenRenderer()
	{
		m_offscreenDSB.setDepthStencilState(DepthStencilState::Type::DepthOnStencilOff);
		g_devcon->OMSetRenderTargets(1, m_offscreenRTV.GetAddressOf(), m_offscreenDSB.getPDepthStencilView());

	}

	void Renderer::initHDRTexture(D3D11_TEXTURE2D_DESC backBufferTex)
	{

		backBufferTex.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		backBufferTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = backBufferTex.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		m_HDRtexture.createTextureFromMemory(backBufferTex, nullptr, srvDesc, PostProcess::POST_PROCESS_SAMPLE_STATE_SLOT);
	}

	void Renderer::setSampleState()
	{

		g_pointWrap.bind();
		g_linearWrap.bind();
		g_anisotropicWrap.bind();

		if (updateConstantBufferSamplerState)
		{
			samplerStateIndexBuffer.setBufferData({ { (UINT)currentSampleState, 0, 0, 0 } });
			updateConstantBufferSamplerState = false;
		}

		samplerStateIndexBuffer.setPixelShaderBuffer();

	}

	void Renderer::initSamplers()
	{
		samplerStateIndexBuffer.initBuffer(SELECTED_SAMPLER_STATE_3, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		samplerStateIndexBuffer.setBufferData({ { (UINT)currentSampleState, 0, 0, 0 } });

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
		g_pointWrap.initSampleState((UINT)SamplerState::BindSlot::POINT_WRAP, samplerDesc);
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		g_linearWrap.initSampleState((UINT)SamplerState::BindSlot::LINEAR_WRAP, samplerDesc);
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		g_anisotropicWrap.initSampleState((UINT)SamplerState::BindSlot::ANISOTROPIC_WRAP, samplerDesc);
	}

	void Renderer::initRasterizerDesc(const D3D11_RASTERIZER_DESC& rasterizerDesc)
	{
		rasterizationDesc = rasterizerDesc;
	}

	void Renderer::createRasterizerState()
	{
		HRESULT result = g_device->CreateRasterizerState(&rasterizationDesc, rasterizerState.ReleaseAndGetAddressOf());

		D3D11_RASTERIZER_DESC transparentRasterizerDesc = rasterizationDesc;
		transparentRasterizerDesc.CullMode = D3D11_CULL_NONE;

		g_device->CreateRasterizerState(&transparentRasterizerDesc, transparentRasterizerState.ReleaseAndGetAddressOf());

		if (FAILED(result))
		{
			PrintError(result, L"Raserizer creation failed: ");
		}
		needToUpdateRasterizer = false;
	}


}