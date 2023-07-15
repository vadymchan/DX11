#pragma once
#include "Buffer.h"

namespace engine::DX
{
	template <typename T>
	class VertexBuffer : public Buffer<T>
	{
	public:

		void initBuffer(UINT startSlot, const std::vector<UINT>& strides, const std::vector<UINT>& offsets, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			this->startSlot = startSlot;
			this->strides = strides;
			this->offsets = offsets;
			Buffer<T>::initBuffer(bufferUsage, D3D11_BIND_VERTEX_BUFFER, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void initBuffer(UINT startSlot, const std::vector<UINT>& strides, const std::vector<UINT>& offsets, D3D11_USAGE bufferUsage, const std::vector<T>& bufferData,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			this->bufferData = bufferData;
			initBuffer(startSlot, strides, offsets, bufferUsage, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void setBuffer() override
		{
			Buffer<T>::setBuffer();
			g_devcon->IASetVertexBuffers(startSlot, 1, Buffer<T>::buffer.GetAddressOf(), strides.data(), offsets.data());
		}

		/// <param name="inputSlot">slot in which buffer binds in Input Assembler</param>
		void setBuffer(UINT inputSlot)
		{
			if (Buffer<T>::updateBuffer)
			{
				Buffer<T>::createBuffer();
			}
			g_devcon->IASetVertexBuffers(inputSlot, 1, Buffer<T>::buffer.GetAddressOf(), strides.data(), offsets.data());
		}

		void updateBuffer(const std::vector<T>& newBufferData)
		{

			if (newBufferData.empty())
			{
				return;
			}

			const UINT oldBufferSize = Buffer<T>::bufferData.size();
			if (oldBufferSize != newBufferData.size()) // new elements added
			{
				Buffer<T>::setBufferData(newBufferData);
				Buffer<T>::createBuffer();
			}
			else
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

				g_devcon->Map(Buffer<T>::buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				memcpy(mappedResource.pData, newBufferData.data(), sizeof(T) * newBufferData.size());
				g_devcon->Unmap(Buffer<T>::buffer.Get(), 0);
			}
		}


		static void setBuffers(UINT startSlot, UINT bufferNum, ID3D11Buffer* const* vertexBuffers, const UINT* strides, const UINT* offsets)
		{
			g_devcon->IASetVertexBuffers(startSlot, bufferNum, vertexBuffers, strides, offsets);
		}

	private:
		UINT startSlot{};
		std::vector<UINT> strides{};
		std::vector<UINT> offsets{};
	};
}