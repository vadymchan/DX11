#pragma once
#include "../D3D/D3D.h"

namespace engine::DX
{
	class DepthStencilBuffer
	{
	public:
		
		void initDepthStencil(UINT bufferWidth, UINT bufferHight,
			D3D11_USAGE depthStencilUsage = D3D11_USAGE_DEFAULT,
			DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT,
			UINT arraySize = 1, UINT mipLevels = 1, UINT CPUAccessFlags = 0,
			UINT miscFlags = 0, UINT sampleQuantity = 1, UINT sampleQuality = 0,
			bool depthEnabled = true, D3D11_COMPARISON_FUNC depthComparison = D3D11_COMPARISON_GREATER,
			D3D11_DEPTH_WRITE_MASK depthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
			bool stencilEnabled = false)
		{
			//backbuffer size
			depthStencilTextureDesc.Width = bufferWidth;
			depthStencilTextureDesc.Height = bufferHight;
			depthStencilTextureDesc.ArraySize = arraySize;
			depthStencilTextureDesc.MipLevels = mipLevels;
			depthStencilTextureDesc.Usage = depthStencilUsage;
			depthStencilTextureDesc.Format = depthStencilFormat;
			depthStencilTextureDesc.CPUAccessFlags = CPUAccessFlags;
			depthStencilTextureDesc.MiscFlags = miscFlags;
			depthStencilTextureDesc.SampleDesc.Count = sampleQuantity;
			depthStencilTextureDesc.SampleDesc.Quality = sampleQuality;
			depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			g_device->CreateTexture2D(&depthStencilTextureDesc, nullptr, depthStencilTexture.ReleaseAndGetAddressOf());

			//depth
			depthStencilDesc.DepthEnable = depthEnabled;
			depthStencilDesc.DepthFunc = depthComparison; 
			depthStencilDesc.DepthWriteMask = depthWriteMask;

			//stencil
			depthStencilDesc.StencilEnable = stencilEnabled;

			//create depth stencil-state
			g_device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.ReleaseAndGetAddressOf());
			
			//bind depthStencilState 
			g_devcon->OMSetDepthStencilState(depthStencilState.Get(), 1);

			//create depth stencil view
			depthStencilViewDesc.Format = depthStencilFormat;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
			depthStencilViewDesc.Flags = 0;

			HRESULT result = g_device->CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, depthStencilView.ReleaseAndGetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth Stencil View was not created!\n");
			}
		}

		ID3D11DepthStencilView* getPDepthStencilView() { return depthStencilView.Get(); }

	private:
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

		// add texture 2d
		ComPtr<ID3D11Texture2D> depthStencilTexture;
		D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
		
		//
		ComPtr<ID3D11DepthStencilState> depthStencilState;
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
	};
}
