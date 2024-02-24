#pragma once
#include "Buffer.h"

namespace engine::DX
{
	struct PerFrameIBL
	{
		bool useDiffuseReflection;
		char padding1[3];
		bool useSpecularReflection;
		char padding2[3];
		bool useIBL;
		char padding3[3];
		bool useRoughnessOverwriting;
		char padding4[3];
		float overwrittenRoughnessValue;
		float3 padding5;
	};


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
			g_devcon->GSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
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

		void setGeometryShaderBuffer()
		{
			Buffer<T>::setBuffer();
			g_devcon->GSSetConstantBuffers(registerSlot, 1, Buffer<T>::buffer.GetAddressOf());
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

		void updateData(const std::vector<T>& bufferData)
		{
			if (Buffer<T>::bufferDescription.ByteWidth != sizeof(T) * bufferData.size()) // new elements added
			{
				Buffer<T>::bufferDescription.ByteWidth = sizeof(T) * bufferData.size();
				Buffer<T>::bufferSubresourceData.pSysMem = bufferData.data();
				Buffer<T>::createBuffer();
			}
			else
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				HRESULT result = g_devcon->Map(Buffer<T>::buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				if (SUCCEEDED(result))
				{
					memcpy(mappedResource.pData, bufferData.data(), bufferData.size() * sizeof(T));
					g_devcon->Unmap(Buffer<T>::buffer.Get(), 0);
				}
				else
				{
					PrintError(result, L"constant buffer data was not updated!");
				}
			}
		}


	private:
		UINT registerSlot;
	};
}
