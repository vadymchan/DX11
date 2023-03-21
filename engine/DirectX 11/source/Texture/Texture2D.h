#pragma once
#include "../D3D/D3D.h"
#include "DDSTextureLoader11.h"


namespace engine::DX
{

	class Texture2D
	{
	public:
		void createTextureFromMemory(const D3D11_TEXTURE2D_DESC& textureDesc, const void* textureData, UINT memoryPitch = 0, UINT memorySlicePitch = 0)
		{
			initTextureDescription(textureDesc);
			initTextureSubresourceData(textureData, memoryPitch, memorySlicePitch);
			if (!textureUpdated)
			{
				g_device->CreateTexture2D(&m_textureDescription, &m_textureSubresourceData , texture.GetAddressOf());
				textureUpdated = true;
			}
		}

		void createTextureFromFile(
			const std::wstring& fileName,
			const D3D11_TEXTURE2D_DESC& textureDesc,
			size_t maxSize = 0,
			DirectX::DDS_LOADER_FLAGS loadFlags = DirectX::DDS_LOADER_DEFAULT,
			DirectX::DDS_ALPHA_MODE* alphaMode = nullptr)
		{

			DirectX::CreateDDSTextureFromFileEx(
				(ID3D11Device*)g_device,
				(ID3D11DeviceContext*)g_devcon,
				fileName.c_str(),
				maxSize,
				textureDesc.Usage,
				(unsigned int)textureDesc.BindFlags,
				(unsigned int)textureDesc.CPUAccessFlags,
				(unsigned int)textureDesc.MiscFlags,
				loadFlags,
				(ID3D11Resource**)m_texture.GetAddressOf(),
				m_shaderResourceView.GetAddressOf(),
				alphaMode);

		
		}


	private:

		void initTextureDescription(const D3D11_TEXTURE2D_DESC& textureDesc )
		{
			m_textureDescription = textureDesc;
			/*m_textureDescription.Width = 0;
			m_textureDescription.Height = 0;
			m_textureDescription.MipLevels = 1;
			m_textureDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			m_textureDescription.ArraySize = 1;
			m_textureDescription.CPUAccessFlags = 0;
			m_textureDescription.MiscFlags = 0;
			m_textureDescription.SampleDesc.Count = 1;
			m_textureDescription.SampleDesc.Quality = 0;
			m_textureDescription.Usage = D3D11_USAGE_DEFAULT;*/

			
		
			textureUpdated = false;
		}

		void initTextureSubresourceData(const void* textureData, UINT memoryPitch = 0, UINT memorySlicePitch = 0)
		{
			m_textureSubresourceData.pSysMem = textureData;
			m_textureSubresourceData.SysMemPitch = memoryPitch;
			m_textureSubresourceData.SysMemSlicePitch = memorySlicePitch;
			textureUpdated = false;
		}



		D3D11_TEXTURE2D_DESC m_textureDescription{};
		D3D11_SUBRESOURCE_DATA m_textureSubresourceData{}; //initial data
		ComPtr<ID3D11Texture2D> m_texture;
		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView{};

		bool textureUpdated{};
	};

}