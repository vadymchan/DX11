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
			void createTextureFromMemory(UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc, const void* textureData,
				const D3D11_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc, UINT memoryPitch = 0, UINT memorySlicePitch = 0)
			{
				m_bindSlot = bindSlot;
				initTextureDescription(textureDesc);
				initShaderResourceView(shaderResourceViewDesc);
				initTextureSubresourceData(textureData, memoryPitch, memorySlicePitch);
				if (!textureUpdated)
				{
					g_device->CreateTexture2D(&m_textureDescription, m_textureSubresourceData.pSysMem ? &m_textureSubresourceData : nullptr, m_texture.GetAddressOf());
					g_device->CreateShaderResourceView(m_texture.Get(), &m_shaderResourceViewDesc, m_shaderResourceView.GetAddressOf());
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

				HRESULT result = DirectX::CreateDDSTextureFromFileEx(
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

				if (FAILED(result))
				{
					PrintError(result, fileName + L" was not created");
				}

				//set texture description
				m_texture->GetDesc(&m_textureDescription);
			}

			const D3D11_TEXTURE2D_DESC& getTextureDesc() const
			{
				return m_textureDescription;
			}

			D3D11_SHADER_RESOURCE_VIEW_DESC* getShaderResourceViewDesc()
			{
				return &m_shaderResourceViewDesc;
			}

			ID3D11ShaderResourceView* const getShaderResourceView() const
			{
				return m_shaderResourceView.Get();
			}

			ID3D11Texture2D* const getTexture2DView()
			{
				return m_texture.Get();
			}

			void bind()
			{
				// may be add VSSetShaderResources if needed in future

				g_devcon->PSSetShaderResources(m_bindSlot, 1, m_shaderResourceView.GetAddressOf());
			}

		private:

			void initShaderResourceView(const D3D11_SHADER_RESOURCE_VIEW_DESC& shaderResourceViewDesc)
			{
				m_shaderResourceViewDesc = shaderResourceViewDesc;
				textureUpdated = false;
			}

			void initTextureDescription(const D3D11_TEXTURE2D_DESC& textureDesc)
			{
				m_textureDescription = textureDesc;
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
			D3D11_SHADER_RESOURCE_VIEW_DESC m_shaderResourceViewDesc{};


			bool textureUpdated{};
		};

	}
}