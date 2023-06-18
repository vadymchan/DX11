//#pragma once
//#include "../D3D/D3D.h"
//
//
//namespace engine::DX
//{
//	class DepthStencilBuffer
//	{
//	public:
//		
//		DepthStencilBuffer() {}
//
//		DepthStencilBuffer(const DepthStencilBuffer& other)
//		{
//			this->m_depthStencilDesc = other.m_depthStencilDesc;
//			this->m_depthStencilTextureDesc = other.m_depthStencilTextureDesc;
//			this->m_depthStencilViewDesc = other.m_depthStencilViewDesc;
//
//			g_device->CreateTexture2D(&m_depthStencilTextureDesc, nullptr, m_depthStencilTexture.GetAddressOf());
//			g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.GetAddressOf());
//			
//			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
//			if (FAILED(result))
//			{
//				PrintError(result, L"Depth Stencil View was not created!\n");
//			}
//
//		}
//
//		DepthStencilBuffer& operator=(const DepthStencilBuffer& other)
//		{
//			if (this == &other)
//			{
//				return *this;
//			}
//
//			this->m_depthStencilDesc = other.m_depthStencilDesc;
//			this->m_depthStencilTextureDesc = other.m_depthStencilTextureDesc;
//			this->m_depthStencilViewDesc = other.m_depthStencilViewDesc;
//
//			g_device->CreateTexture2D(&m_depthStencilTextureDesc, nullptr, m_depthStencilTexture.GetAddressOf());
//			g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.GetAddressOf());
//
//			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
//			if (FAILED(result))
//			{
//				PrintError(result, L"Depth Stencil View was not created!\n");
//			}
//
//			return *this;
//		}
//
//		~DepthStencilBuffer()
//		{
//			m_depthStencilTexture.Reset();
//			m_depthStencilState.Reset();
//			m_depthStencilView.Reset();
//		}
//
//		/// <summary>
//		/// sets depth stencil state to the pipeline
//		/// </summary>
//		void setDepthStencilState()
//		{
//			g_devcon->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
//		}
//
//		/// <summary>
//		/// Pay attention: in depthStencilTextureDesc there's no need to set BindFlags, it's set in initDepthStencil to D3D11_BIND_DEPTH_STENCIL
//		/// </summary
//		void initDepthStencil(const D3D11_TEXTURE2D_DESC& depthStencilTextureDesc,
//			const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc, 
//			const D3D11_DEPTH_STENCIL_VIEW_DESC& depthStencilViewDesc)
//		{
//			//backbuffer size
//			m_depthStencilTextureDesc = depthStencilTextureDesc;
//			m_depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//
//			g_device->CreateTexture2D(&m_depthStencilTextureDesc, nullptr, m_depthStencilTexture.GetAddressOf());
//
//			//depth & stencil desc
//			m_depthStencilDesc = depthStencilDesc;
//
//			//create depth stencil-state
//			g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.GetAddressOf());
//
//			//create depth stencil view
//			m_depthStencilViewDesc = depthStencilViewDesc;
//
//			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
//			if (FAILED(result))
//			{
//				PrintError(result, L"Depth Stencil View was not created!\n");
//			}
//		}
//
//		void resizeDepthStencilTexture(UINT width, UINT height)
//		{
//			m_depthStencilTextureDesc.Width = width;
//			m_depthStencilTextureDesc.Height = height;
//			g_device->CreateTexture2D(&m_depthStencilTextureDesc, nullptr, m_depthStencilTexture.GetAddressOf());
//			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.Get(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
//			if (FAILED(result))
//			{
//				PrintError(result, L"Depth Stencil View was not created!\n");
//			}
//		}
//
//		ID3D11DepthStencilView* getPDepthStencilView() { return m_depthStencilView.Get(); }
//
//	private:
//		D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;
//
//		// use texture2d instead 
//		ComPtr<ID3D11Texture2D> m_depthStencilTexture;
//		D3D11_TEXTURE2D_DESC m_depthStencilTextureDesc;
//		//
//
//
//		ComPtr<ID3D11DepthStencilState> m_depthStencilState;
//		D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
//		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
//	};
//}


#pragma once
#include "../D3D/D3D.h"
#include "../Texture/Texture2D.h"

namespace engine::DX
{
	class DepthStencilBuffer
	{
	public:

		DepthStencilBuffer() {}

		DepthStencilBuffer(const DepthStencilBuffer& other)
		{
			this->m_depthStencilDesc = other.m_depthStencilDesc;
			this->m_depthStencilTexture = other.m_depthStencilTexture;
			this->m_depthStencilViewDesc = other.m_depthStencilViewDesc;

			// Create a new DepthStencilState
			HRESULT result = g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil State was not created!\n");
			}

			result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
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

			this->m_depthStencilDesc = other.m_depthStencilDesc;
			this->m_depthStencilTexture = other.m_depthStencilTexture;
			this->m_depthStencilViewDesc = other.m_depthStencilViewDesc;

			// Create a new DepthStencilState
			HRESULT result = g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil State was not created!\n");
			}

			result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}

			return *this;
		}


		~DepthStencilBuffer()
		{
			m_depthStencilState.Reset();
			m_depthStencilView.Reset();
		}

		/// <summary>
		/// sets depth stencil state to the pipeline
		/// </summary>
		void setDepthStencilState()
		{
			g_devcon->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
		}

		void initDepthStencil(const Texture2D& depthStencilTexture, 
			const D3D11_DEPTH_STENCIL_DESC& depthStencilDesc,
			const D3D11_DEPTH_STENCIL_VIEW_DESC& depthStencilViewDesc)
		{
			//backbuffer size
			m_depthStencilTexture = depthStencilTexture; 

			m_depthStencilDesc = depthStencilDesc;

			//create depth stencil-state
			HRESULT result = g_device->CreateDepthStencilState(&m_depthStencilDesc, m_depthStencilState.GetAddressOf());
			if (FAILED(result)) 
			{
				PrintError(result, L"Depth Stencil State was not created!\n");
			}


			//create depth stencil view
			m_depthStencilViewDesc = depthStencilViewDesc;

			result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}
		}

		void resizeDepthStencilTexture(UINT width, UINT height)
		{
			m_depthStencilTexture.resize(width, height);
			HRESULT result = g_device->CreateDepthStencilView(m_depthStencilTexture.getTexture2DView(), &m_depthStencilViewDesc, m_depthStencilView.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}
		}


		ID3D11DepthStencilView* getPDepthStencilView() { return m_depthStencilView.Get(); }

	private:
		D3D11_DEPTH_STENCIL_DESC m_depthStencilDesc;

		// use texture2d instead 
		Texture2D m_depthStencilTexture;
		//


		ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		D3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	};
}
