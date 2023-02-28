#pragma once

#include "../D3D/D3D.h"
#include <vector>

namespace engine::DX
{
	template <typename T>
	class Buffer
	{
	public:

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

		void createBuffer()
		{
			if (updateBuffer)
			{
				HRESULT result = g_device->CreateBuffer(&bufferDescription, &bufferSubresourceData, buffer.ReleaseAndGetAddressOf());
				if (FAILED(result))
				{
					LPSTR messageBuffer = nullptr;
					size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						nullptr, result, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);
					std::cerr << "Buffer " << buffer.Get() << " was not initialized: " 
						<< std::string(messageBuffer, size) <<"\n";
					LocalFree(messageBuffer);
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
		
	protected:

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
