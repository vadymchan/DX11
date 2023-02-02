#pragma once

#include "../../general/include/win_def.h"


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cstdint>
#include <iostream>


#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "WindowsApp.lib")

using Microsoft::WRL::ComPtr;

namespace engine::DX
{
	//  for have an access to resources
	extern ID3D11Device5* g_device;
	extern ID3D11DeviceContext4* g_devcon;
	extern IDXGIFactory5* g_factory;
	extern ID3D11Debug* g_debug;


	class D3D //for managing resources
	{
	public:
		
		static void Init();
		static void Deinit();

	private:
		static ComPtr<IDXGIFactory> m_factory;
		static ComPtr<IDXGIFactory5> m_factory5;
		static ComPtr<ID3D11Device> m_device;
		static ComPtr<ID3D11Device5> m_device5;
		static ComPtr<ID3D11DeviceContext> m_devCon;
		static ComPtr<ID3D11DeviceContext4> m_devCon4;
		static ComPtr<ID3D11Debug> m_devDebug;

	};


}

#include "../../general/include/win_undef.h"