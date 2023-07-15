#pragma once

#include <array>

#include "../D3D/D3D.h"
#include "../Texture/Texture2D.h"

namespace engine::DX
{

	class DeferredShading
	{
	public:

		enum class GBufferType
		{
			ALBEDO, // Albedo (RGB)
			ROUGHNESS_METALNESS, // Roughness (R), Metalness (G)
			NORMALS, // Texture Normal (RG), Geometry Normal (BA)
			EMISSION, // Emission (RGB)
			OBJECTID, // ObjectID
			GBUFFER_NUM
		};


		DeferredShading(int width, int height)
			: textureWidth{ width }
			, textureHeight{ height }
		{
			CreateGBuffers();
		}

		void Resize(int width, int height)
		{
			textureWidth = width;
			textureHeight = height;
			CreateGBuffers();
		}

		void BeginGeometryPass()
		{
			constexpr int GBUFFER_NUM = (int)GBufferType::GBUFFER_NUM;

			ID3D11RenderTargetView* rtvs[GBUFFER_NUM];
			for (int i = 0; i < GBUFFER_NUM; ++i) {
				rtvs[i] = m_gBufferRTVs[i].Get();
			}
			g_devcon->OMSetRenderTargets(GBUFFER_NUM, rtvs, nullptr);
		}

		void EndGeometryPass()
		{
			constexpr ID3D11RenderTargetView* nullViews[(int)GBufferType::GBUFFER_NUM] = { nullptr, nullptr, nullptr, nullptr, nullptr };
			g_devcon->OMSetRenderTargets((int)GBufferType::GBUFFER_NUM, nullViews, nullptr);
		}

		void BeginLightPass()
		{
			constexpr int GBUFFER_NUM = (int)GBufferType::GBUFFER_NUM;

			ID3D11ShaderResourceView* srvs[(int)GBufferType::GBUFFER_NUM];
			for (int i = 0; i < (int)GBufferType::GBUFFER_NUM; ++i) {
				srvs[i] = m_gBuffers[i].getShaderResourceView();
			}
			g_devcon->PSSetShaderResources(0, GBUFFER_NUM, srvs);
		}

		void EndLightPass()
		{
			constexpr ID3D11ShaderResourceView* nullViews[(int)GBufferType::GBUFFER_NUM] = { nullptr, nullptr, nullptr, nullptr, nullptr };
			g_devcon->PSSetShaderResources(0, (int)GBufferType::GBUFFER_NUM, nullViews);
		}


	private:

		int textureWidth;
		int textureHeight;

		std::array<Texture2D, (int)GBufferType::GBUFFER_NUM> m_gBuffers;
		std::array<ComPtr<ID3D11RenderTargetView>, (int)GBufferType::GBUFFER_NUM> m_gBufferRTVs;

		void CreateGBuffers()
		{
			CreateGBuffer(GBufferType::ALBEDO, DXGI_FORMAT_R8G8B8A8_UNORM);
			CreateGBuffer(GBufferType::ROUGHNESS_METALNESS, DXGI_FORMAT_R8G8_UNORM);
			CreateGBuffer(GBufferType::NORMALS, DXGI_FORMAT_R16G16B16A16_SNORM);
			CreateGBuffer(GBufferType::EMISSION, DXGI_FORMAT_R16G16B16A16_FLOAT);
			CreateGBuffer(GBufferType::OBJECTID, DXGI_FORMAT_R16_UINT);
		}

		void CreateGBuffer(GBufferType type, DXGI_FORMAT format)
		{
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = textureWidth;
			textureDesc.Height = textureHeight;
			textureDesc.MipLevels = 1;
			textureDesc.ArraySize = 1;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = 0;
			textureDesc.Format = format;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			m_gBuffers[(int)type].createTextureFromMemory(textureDesc, nullptr, srvDesc);

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvDesc.Texture2D.MipSlice = 0;
			rtvDesc.Format = format;

			g_device->CreateRenderTargetView(m_gBuffers[(int)type].getTexture2DView(), &rtvDesc, m_gBufferRTVs[(int)type].GetAddressOf());
		}


		//void CreateGBuffers(int width, int height)
		//{
		//    D3D11_TEXTURE2D_DESC textureDesc = {};
		//    textureDesc.Width = width;
		//    textureDesc.Height = height;
		//    textureDesc.MipLevels = 1;
		//    textureDesc.ArraySize = 1;
		//    textureDesc.SampleDesc.Count = 1;
		//    textureDesc.SampleDesc.Quality = 0;
		//    textureDesc.Usage = D3D11_USAGE_DEFAULT;
		//    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		//    textureDesc.CPUAccessFlags = 0;
		//    textureDesc.MiscFlags = 0;

		//    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		//    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//    rtvDesc.Texture2D.MipSlice = 0;

		//    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		//    srvDesc.Format = textureDesc.Format;
		//    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//    srvDesc.Texture2D.MostDetailedMip = 0;
		//    srvDesc.Texture2D.MipLevels = 1;

		//    // Albedo
		//    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//    rtvDesc.Format = textureDesc.Format;
		//    m_gBuffers[(int)GBufferType::ALBEDO].createTextureFromMemory(textureDesc, nullptr, srvDesc);
		//    g_device->CreateRenderTargetView(m_gBuffers[(int)GBufferType::ALBEDO].getTexture2DView(), &rtvDesc, m_gBufferRTVs[(int)GBufferType::ALBEDO].GetAddressOf());

		//    // Roughness, Metalness
		//    textureDesc.Format = DXGI_FORMAT_R8G8_UNORM;
		//    rtvDesc.Format = textureDesc.Format;
		//    m_gBuffers[(int)GBufferType::ROUGHNESS_METALNESS].createTextureFromMemory(textureDesc, nullptr, srvDesc);
		//    g_device->CreateRenderTargetView(m_gBuffers[(int)GBufferType::ROUGHNESS_METALNESS].getTexture2DView(), &rtvDesc, m_gBufferRTVs[(int)GBufferType::ROUGHNESS_METALNESS].GetAddressOf());

		//    // Normals
		//    textureDesc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
		//    rtvDesc.Format = textureDesc.Format;
		//    m_gBuffers[(int)GBufferType::NORMALS].createTextureFromMemory(textureDesc, nullptr, srvDesc);
		//    g_device->CreateRenderTargetView(m_gBuffers[(int)GBufferType::NORMALS].getTexture2DView(), &rtvDesc, m_gBufferRTVs[(int)GBufferType::NORMALS].GetAddressOf());

		//    // Emission
		//    textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		//    rtvDesc.Format = textureDesc.Format;
		//    m_gBuffers[(int)GBufferType::EMISSION].createTextureFromMemory(textureDesc, nullptr, srvDesc);
		//    g_device->CreateRenderTargetView(m_gBuffers[(int)GBufferType::EMISSION].getTexture2DView(), &rtvDesc, m_gBufferRTVs[(int)GBufferType::EMISSION].GetAddressOf());

		//    // ObjectID
		//    textureDesc.Format = DXGI_FORMAT_R16_UINT;
		//    rtvDesc.Format = textureDesc.Format;
		//    m_gBuffers[(int)GBufferType::OBJECTID].createTextureFromMemory(textureDesc, nullptr, srvDesc);
		//    g_device->CreateRenderTargetView(m_gBuffers[(int)GBufferType::OBJECTID].getTexture2DView(), &rtvDesc, m_gBufferRTVs[(int)GBufferType::OBJECTID].GetAddressOf());
		//}

	};

}