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
				m_samplerDesc = desc;
				m_bindSlot = bindSlot;

				g_device->CreateSamplerState(&m_samplerDesc, m_samplerState.GetAddressOf());
			}

			void bind()
			{
				g_devcon->PSSetSamplers(m_bindSlot, 1, m_samplerState.GetAddressOf());
			}

			enum class BindSlot
			{
				POINT_WRAP,
				LINEAR_WRAP,
				ANISOTROPIC_WRAP,
			};

			//as an alternative to the enum class BindSlot
			/*const static UINT g_pointBindSlot;
			const static UINT g_linearBindSlot;
			const static UINT g_anisotropicBindSlot;*/

		private:

			UINT m_bindSlot;
			D3D11_SAMPLER_DESC m_samplerDesc;
			ComPtr<ID3D11SamplerState> m_samplerState;

			

		};



	}

}