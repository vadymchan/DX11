#include "D3D.h"


namespace engine::DX
{
	ID3D11Device5* g_device = nullptr;
	ID3D11DeviceContext4* g_devcon = nullptr;
	IDXGIFactory5* g_factory = nullptr;
	ID3D11Debug* g_debug = nullptr;

	// for using static variables in cpp file
	ComPtr<IDXGIFactory> D3D::m_factory;
	ComPtr<IDXGIFactory5> D3D::m_factory5;
	ComPtr<ID3D11Device> D3D::m_device;
	ComPtr<ID3D11Device5> D3D::m_device5;
	ComPtr<ID3D11DeviceContext> D3D::m_devCon;
	ComPtr<ID3D11DeviceContext4> D3D::m_devCon4;
	ComPtr<ID3D11Debug> D3D::m_devDebug;

	extern "C"
	{
		_declspec(dllexport) uint32_t NvOptimusEnablement = 1;
		_declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
	}

	void D3D::Init()
	{
		if (m_device5 != nullptr && m_devCon4 != nullptr && m_factory5 != nullptr)
		{
			return;
		}
		HRESULT result;

		result = CreateDXGIFactory(_uuidof(IDXGIFactory5), (void**)m_factory5.GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Factory creation faied\n";
		}

		{
			uint32_t index = 0;
			IDXGIAdapter1* adapter;
			while (m_factory5->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);
				std::cout << "GPU #" << index << desc.Description << std::endl; 
			}
		}

		const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_DEBUG, &featureLevelRequested, 1, D3D11_SDK_VERSION, m_device.GetAddressOf(), &featureLevelInitialized, m_devCon.GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Device creation failed\n";
		}
		if (featureLevelInitialized != featureLevelRequested)
		{
			std::cerr << "Requested Level and Initialized Level are different\n";
		}

		result = m_device.Get()->QueryInterface(__uuidof(ID3D11Device5), (void**)m_device5.GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Failed to query ID3D11Device5\n";

		}
		result = m_devCon.Get()->QueryInterface(__uuidof(ID3D11DeviceContext4), (void**)m_devCon4.GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Failed to query ID3D11DeviceContext4\n";
		}
		result = m_device.Get()->QueryInterface(__uuidof(ID3D11Debug), (void**)m_devDebug.GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Failed to query ID3D11Debug\n";
		}
		g_device = m_device5.Get();
		g_devcon = m_devCon4.Get();
		g_factory = m_factory5.Get();
		g_debug = m_devDebug.Get();
	}

	void D3D::Deinit()
	{
		m_factory.Reset();
		m_factory5.Reset();
		m_device.Reset();
		m_device5.Reset();
		m_devCon.Reset();
		m_devCon4.Reset();
		m_devDebug.Reset();
	}
}


