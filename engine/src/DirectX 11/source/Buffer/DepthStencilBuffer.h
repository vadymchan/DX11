#pragma once
#include "../D3D/D3D.h"
#include "../Texture/Texture2D.h"


#include <unordered_map>

namespace engine::DX
{

	class DepthStencilState
	{
	public:
		
		enum class Type
		{
			DepthOnStencilOff, 
			DepthOnStencilOn,
			Count
		};

		DepthStencilState() = default;

		DepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
		{
			m_desc = desc;
			HRESULT result = g_device->CreateDepthStencilState(&m_desc, m_state.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil State was not created!\n");
			}
		}

		DepthStencilState(const DepthStencilState& other)
		{
			m_desc = other.m_desc;
			HRESULT result = g_device->CreateDepthStencilState(&m_desc, m_state.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil State was not created!\n");
			}
		}

		DepthStencilState& operator=(const DepthStencilState& other)
		{
			if (this != &other)
			{
				m_desc = other.m_desc;
				HRESULT result = g_device->CreateDepthStencilState(&m_desc, m_state.ReleaseAndGetAddressOf());
				if (FAILED(result))
				{
					PrintError(result, L"Depth Stencil State was not created!\n");
				}
			}
			return *this;
		}

		ID3D11DepthStencilState* getState() const { return m_state.Get(); }

	private:
		D3D11_DEPTH_STENCIL_DESC m_desc;
		ComPtr<ID3D11DepthStencilState> m_state;
	};


	class DepthStencilBuffer
	{
	public:

		DepthStencilBuffer() {}

		DepthStencilBuffer(const DepthStencilBuffer& other)
		{
			//this->m_depthStencilDesc = other.m_depthStencilDesc;
			this->m_depthStencilTexture = other.m_depthStencilTexture;
			this->m_depthStencilViewDesc = other.m_depthStencilViewDesc;
			this->m_states = other.m_states;

			// Create a new DepthStencilState
			//HRESULT result = g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.ReleaseAndGetAddressOf());
			//if (FAILED(result))
			//{
			//	PrintError(result, L"Depth Stencil State was not created!\n");
			//}

			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}
		}

		DepthStencilBuffer& operator=(const DepthStencilBuffer& other)
		{
			if (this == &other)
			{
				return *this;
			}

			//this->m_depthStencilDesc = other.m_depthStencilDesc;
			this->m_depthStencilTexture = other.m_depthStencilTexture; 
			this->m_depthStencilViewDesc = other.m_depthStencilViewDesc;
			this->m_states = other.m_states;

			// Release old resources and create new DepthStencilState
			//result = g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.ReleaseAndGetAddressOf());
			//if (FAILED(result))
			//{
			//	PrintError(result, L"Depth Stencil State was not created!\n");
			//}

			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}

			return *this;
		}



		~DepthStencilBuffer()
		{
			//m_depthStencilState.Reset();
			m_depthStencilView.Reset();
		}


		//void setDepthStencilState()
		//{
		//	g_devcon->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
		//}


		void setDepthStencilState(DepthStencilState::Type type, UINT StencilRef = 1)
		{
			auto test = m_states[type].getState();
			g_devcon->OMSetDepthStencilState(m_states[type].getState(), StencilRef);
		}

		void initDepthStencil(const Texture2D& depthStencilTexture,
			const std::vector<std::pair<DepthStencilState::Type, D3D11_DEPTH_STENCIL_DESC>>& depthStencilDescs,
			const D3D11_DEPTH_STENCIL_VIEW_DESC& depthStencilViewDesc)
		{
			//backbuffer size
			m_depthStencilTexture = depthStencilTexture; 

			//m_depthStencilDesc = depthStencilDesc;

			//create depth stencil-state
			//HRESULT result = g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.ReleaseAndGetAddressOf());
			//if (FAILED(result)) 
			//{
			//	PrintError(result, L"Depth Stencil State was not created!\n");
			//}

			//create depth stencil-state
			for (const auto& pair : depthStencilDescs) {
				m_states[pair.first] = DepthStencilState(pair.second);
			}

			//create depth stencil view
			m_depthStencilViewDesc = depthStencilViewDesc;

			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}
		}

		void resizeDepthStencilTexture(UINT width, UINT height)
		{
			m_depthStencilTexture.resize(width, height);
			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}
		}


		ID3D11DepthStencilView* getPDepthStencilView() const { return m_depthStencilView.Get(); }
		const Texture2D& getTexture2D() const { return m_depthStencilTexture; }

	private:
		//D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;

		Texture2D m_depthStencilTexture;

		//ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		
		std::unordered_map<DepthStencilState::Type, DepthStencilState> m_states;

		//std::array<DepthStencilState, (int)DepthStencilState::Type::Count> m_states;

		D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	};
}
