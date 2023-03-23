#pragma once
#include "../D3D/D3D.h"

namespace engine
{
	namespace DX
	{

		class SampleState
		{
		public:

			void initSampleState(UINT bindSlot, const D3D11_SAMPLER_DESC& desc)
			{

				m_samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				m_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				m_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
				m_samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				m_samplerDesc.MipLODBias = 0.0f;
				m_samplerDesc.MaxAnisotropy = 1;
				m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
				m_samplerDesc.BorderColor[0] = 0.0f;
				m_samplerDesc.BorderColor[1] = 0.0f;
				m_samplerDesc.BorderColor[2] = 0.0f;
				m_samplerDesc.BorderColor[3] = 0.0f;
				m_samplerDesc.MinLOD = 0;
				m_samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

				m_bindSlot = bindSlot;

				g_device->CreateSamplerState(&m_samplerDesc, m_samplerState.GetAddressOf());
			}

			void bind()
			{
				g_devcon->PSSetSamplers(m_bindSlot, 1, m_samplerState.GetAddressOf());
			}
		private:



			UINT m_bindSlot;
			D3D11_SAMPLER_DESC m_samplerDesc;
			ComPtr<ID3D11SamplerState> m_samplerState;

		};



	}

}