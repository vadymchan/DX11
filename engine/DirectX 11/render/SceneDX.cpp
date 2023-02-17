#include "SceneDX.h"
#include <string>





namespace engine::DX
{
	const std::wstring pathToShaders { L"engine\\general\\resources\\shaders\\" };

	void Scene::render(Window& window)
	{
		window.clearWindow();
		window.setViews();


		constBufferData.time.x = timer.frameTimeElapsed() * timer.milliToSec;


		constBufferData.viewport.x = window.GetViewport().Width;
		constBufferData.viewport.y = window.GetViewport().Height;
			

		D3D11_MAPPED_SUBRESOURCE mapSubresource;
		g_devcon->Map(constBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapSubresource);
		memcpy(mapSubresource.pData, &constBufferData, sizeof(ConstBufferType));
		g_devcon->Unmap(constBuffer.Get(), NULL);
		g_devcon->PSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());

		UINT strides[]
		{
			sizeof(Position), sizeof(Color)
		};

		UINT offsets[]
		{
			0, sizeof(DirectX::XMFLOAT4)
		};

		//g_devcon->IASetVertexBuffers(0,1,vertexBuffer.GetAddressOf(), &stride, &offset);
		g_devcon->IASetVertexBuffers(0,1,vertexBuffer[0].GetAddressOf(), &strides[0], &offsets[0]);
		g_devcon->IASetVertexBuffers(1,1,vertexBuffer[1].GetAddressOf(), &strides[1], &offsets[1]);
		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_devcon->Draw(3, 0);

		window.flush();

	}


	void Scene::initShaders()
	{
		
		HRESULT result = D3DCompileFromFile((pathToShaders + L"vertexShaderold.hlsl").c_str(), 0, 0, "main", "vs_4_0", 0, 0, &VSbinary, 0);
		if (FAILED(result))
		{
			std::cerr << "Vertex shader was not initialized\n";
		}
		result = D3DCompileFromFile((pathToShaders + L"pixelShader.hlsl").c_str(), 0, 0, "main", "ps_4_0", 0, 0, &PSbinary, 0);
		if (FAILED(result))
		{
			std::cerr << "Pixel shader was not initialized\n";
		}

		g_device->CreateVertexShader(VSbinary->GetBufferPointer(), VSbinary->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
		g_device->CreatePixelShader(PSbinary->GetBufferPointer(), PSbinary->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
		
		g_devcon->VSSetShader(vertexShader.Get(), 0, 0);
		g_devcon->PSSetShader(pixelShader.Get(), 0, 0);

		initConstantBuffer();
		
	}

	void Scene::initConstantBuffer()
	{
		//constant buffer
		D3D11_BUFFER_DESC constBufferDesc;
		constBufferDesc.ByteWidth = sizeof(ConstBufferType);
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = 0;
		constBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &constBufferData;
		initData.SysMemPitch = 0;
		initData.SysMemSlicePitch = 0;

		HRESULT result = g_device->CreateBuffer(&constBufferDesc, &initData, constBuffer.GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Constant buffer was not initialized\n";
		}
		g_devcon->PSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());
	}

	void Scene::initInputLayout()
	{
		D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		g_device->CreateInputLayout(ied, 2, VSbinary->GetBufferPointer(), VSbinary->GetBufferSize(), inputLayout.GetAddressOf());
		g_devcon->IASetInputLayout(inputLayout.Get());
	}

	void Scene::initPipeline()
	{
		initShaders();
		initInputLayout();
	}

	void Scene::initVertexBuffer()
	{
		vertexBuffer.push_back(nullptr);
		vertexBuffer.push_back(nullptr);

		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&bufferDescription, sizeof(bufferDescription));

		bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDescription.ByteWidth = sizeof(Position) * positions.size();
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA bufferData;
		bufferData.pSysMem = positions.data();
		bufferData.SysMemPitch = 0;
		bufferData.SysMemSlicePitch = 0;

		HRESULT result = g_device->CreateBuffer(&bufferDescription, &bufferData, vertexBuffer[0].GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Vertex buffer was not inited\n";
		}


		D3D11_BUFFER_DESC bufferDescription1;
		ZeroMemory(&bufferDescription1, sizeof(bufferDescription1));

		bufferDescription1.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDescription1.ByteWidth = sizeof(Color) * colors.size();
		bufferDescription1.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA bufferData1;
		bufferData1.pSysMem = colors.data();
		bufferData1.SysMemPitch = 0;
		bufferData1.SysMemSlicePitch = 0;

		result = g_device->CreateBuffer(&bufferDescription1, &bufferData1, vertexBuffer[1].GetAddressOf());
		if (FAILED(result))
		{
			std::cerr << "Vertex buffer was not inited\n";
		}
	}


}
