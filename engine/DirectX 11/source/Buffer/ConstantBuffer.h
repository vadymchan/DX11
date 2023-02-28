#pragma once
#include "Buffer.h"

namespace engine::DX
{
	template<typename T>
	class ConstantBuffer : public Buffer<T>
	{
	public:

		void initBuffer(UINT registerSlot, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			this->registerSlot = registerSlot;
			Buffer<T>::initBuffer(bufferUsage,
				D3D11_BIND_CONSTANT_BUFFER, CPUAccessFlags,
				miscFlags, structureByteStride,
				sysMemPitch, sysMemSlicePitch);
		}

		/// <summary>
		/// sets constant buffer both to vertex and pixel shader. 
		/// Attention: the slot (register) should be identical both in vertex and pixel shaders!
		/// </summary>
		void setBuffer() override
		{
			Buffer<T>::setBuffer();
			g_devcon->VSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
			g_devcon->PSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
		}

		void setPixelShaderBuffer()
		{
			Buffer<T>::setBuffer();
			g_devcon->PSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
		}

		void setVertexShaderBuffer()
		{
			Buffer<T>::setBuffer();
			g_devcon->VSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
		}

		void setComputeShaderBuffer()
		{
			Buffer<T>::setBuffer();
			g_devcon->CSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
		}


		static void setPixelShaderBuffers(UINT startSlot, UINT numBuffers, ID3D11Buffer* const* constantBuffers)
		{
			Buffer<T>::setBuffer();
			g_devcon->PSSetConstantBuffers(startSlot, numBuffers, constantBuffers);
		}

		static void setVertexShaderBuffers(UINT startSlot, UINT numBuffers, ID3D11Buffer* const* constantBuffers)
		{
			Buffer<T>::setBuffer();
			g_devcon->VSSetConstantBuffers(startSlot, numBuffers, constantBuffers);
		}

		static void setComputeShaderBuffers(UINT startSlot, UINT numBuffers, ID3D11Buffer* const* constantBuffers)
		{
			Buffer<T>::setBuffer();
			g_devcon->CSSetConstantBuffers(startSlot, numBuffers, constantBuffers);
		}

		/// <summary>
		/// sets constant buffers both to vertex and pixel shader. 
		/// Attention: the slot (register) should be identical both in vertex and pixel shaders!
		/// </summary>
		static void setBuffers(UINT startSlot, UINT numBuffers, ID3D11Buffer* const* constantBuffers)
		{
			Buffer<T>::setBuffer();
			g_devcon->VSSetConstantBuffers(startSlot, numBuffers, constantBuffers);
			g_devcon->PSSetConstantBuffers(startSlot, numBuffers, constantBuffers);
		}
	private:
		UINT registerSlot;
	};
}
