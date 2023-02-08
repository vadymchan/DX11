#pragma once
#include "Buffer.h"

namespace engine::DX
{
	template <typename T>
	class VertexBuffer : public Buffer<T>
	{
	public:

		void initBuffer(UINT startSlot, UINT stride, UINT offset, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			this->startSlot = startSlot;
			this->stride = stride;
			this->offset = offset;
			Buffer<T>::initBuffer(bufferUsage, D3D11_BIND_VERTEX_BUFFER, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void initBuffer(UINT startSlot, UINT stride, UINT offset, D3D11_USAGE bufferUsage, const std::vector<T>& bufferData,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			this->bufferData = bufferData;
			initBuffer(startSlot, stride, offset, bufferUsage, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		//temporary here. May be deleted later
		//---------------------------------------
		UINT GetInputSlot() const
		{
			return startSlot;
		}

		void SetInputSlot(UINT value)
		{
			startSlot = value;
		}

		void setBuffer() override
		{
			Buffer<T>::setBuffer();
			g_devcon->IASetVertexBuffers(startSlot, 1, Buffer<T>::buffer.GetAddressOf(), &stride, &offset);
		}
		//---------------------------------------


		/// <param name="inputSlot">slot in which buffer binds in Input Assembler</param>
		void setBuffer(UINT inputSlot)
		{
			if (Buffer<T>::updateBuffer)
			{
				Buffer<T>::createBuffer();
			}
			g_devcon->IASetVertexBuffers(inputSlot, 1, Buffer<T>::buffer.GetAddressOf(), &stride, &offset);
		}

		static void setBuffers(UINT startSlot, UINT bufferNum, ID3D11Buffer*const* vertexBuffers, const UINT* strides, const UINT* offsets)
		{
			g_devcon->IASetVertexBuffers(startSlot, bufferNum, vertexBuffers, strides, offsets);
		}

	private:
		UINT startSlot{};
		UINT stride{};
		UINT offset{};
	};
}