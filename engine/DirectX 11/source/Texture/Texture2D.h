#pragma once
#include "../D3D/D3D.h"
#include "DDSTextureLoader11.h"


namespace engine
{
	namespace DX
	{


		class Texture2D
		{
		public:
			/// <param name="bindSlot">in which register in shader to bind</param>
			void createTextureFromMemory(UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc, const void* textureData, UINT memoryPitch = 0, UINT memorySlicePitch = 0)
			{
				m_bindSlot = bindSlot;
				initTextureDescription(textureDesc);
				initTextureSubresourceData(textureData, memoryPitch, memorySlicePitch);
				if (!textureUpdated)
				{
					g_device->CreateTexture2D(&m_textureDescription, &m_textureSubresourceData, m_texture.GetAddressOf());
					textureUpdated = true;
				}
			}

			/// <param name="bindSlot">in which register in shader to bind</param>
			/// <param name="textureDesc">only need to set BindFlags, CPUAccessFlags and MiscFlags </param>
			void createTextureFromFile(
				UINT bindSlot,
				const std::wstring& fileName,
				const D3D11_TEXTURE2D_DESC& textureDesc,
				size_t maxSize = 0,
				DirectX::DDS_LOADER_FLAGS loadFlags = DirectX::DDS_LOADER_DEFAULT,
				DirectX::DDS_ALPHA_MODE* alphaMode = nullptr)
			{
				m_bindSlot = bindSlot;

				DirectX::CreateDDSTextureFromFileEx(
					g_device,
					g_devcon,
					fileName.c_str(),
					maxSize,
					textureDesc.Usage,
					textureDesc.BindFlags,
					textureDesc.CPUAccessFlags,
					textureDesc.MiscFlags,
					loadFlags,
					(ID3D11Resource**)m_texture.GetAddressOf(),
					m_shaderResourceView.GetAddressOf(),
					alphaMode);

				//set texture description
				m_texture->GetDesc(&m_textureDescription);

				//set texture subresource
				D3D11_MAPPED_SUBRESOURCE subresource;
				g_devcon->Map(m_texture.Get(), 0, D3D11_MAP_READ, 0, &subresource);
				m_textureSubresourceData.pSysMem = subresource.pData;
				m_textureSubresourceData.SysMemPitch = subresource.RowPitch;
				m_textureSubresourceData.SysMemSlicePitch = subresource.DepthPitch;
				g_devcon->Unmap(m_texture.Get(), 0);
			}


			void bind()
			{
				// may be add VSSetShaderResources if needed in future

				g_devcon->PSSetShaderResources(m_bindSlot, 1, m_shaderResourceView.GetAddressOf());
			}

		private:

			void initTextureDescription(const D3D11_TEXTURE2D_DESC& textureDesc)
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


			UINT m_bindSlot{};
			D3D11_TEXTURE2D_DESC m_textureDescription{};
			D3D11_SUBRESOURCE_DATA m_textureSubresourceData{};
			ComPtr<ID3D11Texture2D> m_texture;
			ComPtr<ID3D11ShaderResourceView> m_shaderResourceView{};

			bool textureUpdated{};
		};

	}
}