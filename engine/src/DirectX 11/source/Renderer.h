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
			TextureManager::getInstance().getSamplerState(L"g_pointWrap").bind();
			TextureManager::getInstance().getSamplerState(L"g_linearWrap").bind();
			TextureManager::getInstance().getSamplerState(L"g_anisotropicWrap").bind();
			MeshSystem::getInstance().render(camera, visualizeNormal);

			renderSkybox(camera);
			
			window.flush();
			window.clearDepthStencil();
		}

		/// <summary>
		/// Pay attention - you don't need to create skybox texture via TextureManager::getInstance().addTexture2D() before calling this function
		/// </summary>
		void initRasterizator(const D3D11_RASTERIZER_DESC& rasterizerDesc, const std::wstring& skyboxTextureFileName = L"")
		{
			initRasterizerDesc(rasterizerDesc);
			createRasterizerState();

			initSamplers();
			

			initSkybox(skyboxTextureFileName);
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

		void changeSkybox(const std::wstring& textureFileName)
		{
			m_skyboxTextureFileName = textureFileName;
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

		void renderSkybox(Camera& camera)
		{
			camera.setInvCameraBufferVertexShader();
			ShaderManager::getInstance().getVertexShader(L"skyboxVertexShader.hlsl")->bind();
			ShaderManager::getInstance().getPixelShader(L"skyboxPixelShader.hlsl")->bind();
			g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			g_devcon->HSSetShader(nullptr, nullptr, 0);
			g_devcon->DSSetShader(nullptr, nullptr, 0);
			g_devcon->GSSetShader(nullptr, nullptr, 0);

			TextureManager::getInstance().getTexture2D(m_skyboxTextureFileName).bind();
			g_devcon->Draw(3, 0);
		}

		void initSkybox(const std::wstring& skyboxTextureFileName)
		{
			m_skyboxTextureFileName = skyboxTextureFileName;
			//initing skybox shaders

			std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout {};
			ShaderManager::getInstance().addVertexShader(L"skyboxVertexShader.hlsl", inputLayout);
			ShaderManager::getInstance().addPixelShader(L"skyboxPixelShader.hlsl");

			//initing skybox texture
			if (!skyboxTextureFileName.empty())
			{
				D3D11_TEXTURE2D_DESC textureDesc;
				textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				textureDesc.Usage = D3D11_USAGE_DEFAULT;
				textureDesc.CPUAccessFlags = 0;
				textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
				TextureManager::getInstance().addTexture2D(skyboxTextureFileName, 0, textureDesc);
			}

		}

		void initSamplers()
		{
			D3D11_SAMPLER_DESC samplerDesc;
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MipLODBias = 0.0f;
			samplerDesc.MaxAnisotropy = 1;
			samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			samplerDesc.BorderColor[0] = 0.0f;
			samplerDesc.BorderColor[1] = 0.0f;
			samplerDesc.BorderColor[2] = 0.0f;
			samplerDesc.BorderColor[3] = 0.0f;
			samplerDesc.MinLOD = 0;
			samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			TextureManager::getInstance().addSamplerState(L"g_pointWrap", 0, samplerDesc);
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			TextureManager::getInstance().addSamplerState(L"g_linearWrap", 1, samplerDesc);
			samplerDesc.MaxAnisotropy = 16;
			samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			TextureManager::getInstance().addSamplerState(L"g_anisotropicWrap", 2, samplerDesc);
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



		D3D11_RASTERIZER_DESC rasterizationDesc;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		std::wstring m_skyboxTextureFileName;
		bool visualizeNormal{};
		bool needToUpdateRasterizer{};


	};

}