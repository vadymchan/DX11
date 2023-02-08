#pragma once

#include "../D3D/D3D.h"
#include <vector>

namespace engine::DX
{
	template <typename T>
	class Buffer
	{
	public:

		//void addElement(T value)
		//{
		//	bufferData.push_back(value);
		//	updateBuffer = true;
		//}

		void addElements(const std::vector<T>& bufferData)
		{
			this->bufferData.reserve(this->bufferData.size() + bufferData.size());
			this->bufferData.insert(end(this->bufferData), begin(bufferData), end(bufferData));
			updateBuffer = true;
		}

		void setBufferData(const std::vector<T>& bufferData)
		{
			this->bufferData = bufferData;
			updateBuffer = true;
		}

		const std::vector<T>& getBufferData() const
		{
			return bufferData;
		}

		/*void setSizeOfBuffer(UINT value)
		{
			bufferDescription.ByteWidth = value;
			updateBuffer = true;
		}*/

		//void setBufferData(const void* pData)
		//{
		//	bufferSubresourceData.pSysMem = pData;
		//	updateBuffer = true;
		//}

		void createBuffer()
		{
			if (updateBuffer)
			{
				HRESULT result = g_device->CreateBuffer(&bufferDescription, &bufferSubresourceData, buffer.ReleaseAndGetAddressOf());
				if (FAILED(result))
				{
					std::cerr << "Buffer " << buffer.Get() << " was not initialized\n";
				}
				updateBuffer = false;
			}
		}

		bool empty() const { return bufferDescription.ByteWidth == 0; }
		virtual void setBuffer()
		{
			if (updateBuffer)
			{
				bufferDescription.ByteWidth = sizeof(T) * bufferData.size();
				bufferSubresourceData.pSysMem = bufferData.data();
				createBuffer();
			}

		};
		//virtual void setBuffers(UINT startSlot, UINT bufferNum, ID3D11Buffer* const* vertexBuffers, const UINT* strides, const UINT* offsets) = 0;

	protected:

		//void initBuffer(/*UINT sizeOfBuffer,*/ D3D11_USAGE bufferUsage, const std::vector<T>& bufferData, /*const void* pData,*/
		//	UINT bindFlags = 0, UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
		//	UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		//{
		//	this->bufferData = bufferData;
		//	initBuffer(bufferUsage, bindFlags, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		//}

		void initBuffer(D3D11_USAGE bufferUsage,
			UINT bindFlags = 0, UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			initBufferDescription(bufferUsage, bindFlags, CPUAccessFlags, miscFlags, structureByteStride);
			initBufferData(sysMemPitch, sysMemSlicePitch);
			updateBuffer = true;
		}

		void initBufferDescription(D3D11_USAGE bufferUsage, UINT bindFlags = 0, UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0)
		{
			bufferDescription.ByteWidth = sizeof(T) * bufferData.size();
			bufferDescription.Usage = bufferUsage;
			bufferDescription.BindFlags = bindFlags;
			bufferDescription.CPUAccessFlags = CPUAccessFlags;
			bufferDescription.MiscFlags = miscFlags;
			bufferDescription.StructureByteStride = structureByteStride;
		}

		void initBufferData(UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{

			bufferSubresourceData.pSysMem = bufferData.data();
			bufferSubresourceData.SysMemPitch = sysMemPitch;
			bufferSubresourceData.SysMemSlicePitch = sysMemSlicePitch;

		}



		D3D11_BUFFER_DESC bufferDescription{};
		D3D11_SUBRESOURCE_DATA bufferSubresourceData{};
		ComPtr<ID3D11Buffer> buffer;
		bool updateBuffer{ true };
		std::vector<T> bufferData;

	};

}
