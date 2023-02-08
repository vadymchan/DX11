#pragma once
#include <cstdint>
#include <vector>
#include "Buffer.h"

namespace engine::DX
{
	class IndexBuffer : public Buffer<uint32_t>
	{
	public:


		/// <summary>
		/// Attention: use this method only after you added all indices to the buffer.
		/// Otherwise, the buffer will be reallocated during the setting to pipeline 
		/// </summary>
		void initBuffer(D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			Buffer::initBuffer(bufferUsage, D3D11_BIND_INDEX_BUFFER, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void initBuffer(D3D11_USAGE bufferUsage, const std::vector<uint32_t>& bufferData,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			this->bufferData = bufferData;
			Buffer::initBuffer(bufferUsage, D3D11_BIND_INDEX_BUFFER, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void setBuffer() override
		{
			Buffer<uint32_t>::setBuffer();
			g_devcon->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		}

	};
}
