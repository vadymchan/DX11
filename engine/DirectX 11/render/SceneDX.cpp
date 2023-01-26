#include "SceneDX.h"
#include <string>





namespace DX::engine
{
	const std::wstring pathToShaders { L"..\\..\\engine\\DirectX 11\\shaders\\" };

	void Scene::render(Window& window)
	{
		window.clearWindow();
		g_devcon->OMSetRenderTargets(1, window.GetppRenderTargetView()	, nullptr);


		constBufferData.time.x = timer.frameTimeElapsed() * 0.001f;


		constBufferData.viewport.x = window.GetViewport().Width;
		constBufferData.viewport.y = window.GetViewport().Height;
			

		D3D11_MAPPED_SUBRESOURCE mapSubresource;
		g_devcon->Map(constBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapSubresource);
		memcpy(mapSubresource.pData, &constBufferData, sizeof(ConstBuffer));
		g_devcon->Unmap(constBuffer.Get(), NULL);
		g_devcon->PSSetConstantBuffers(0, 1, constBuffer.GetAddressOf());


		g_devcon->IASetVertexBuffers(0,1,vertexBuffer.GetAddressOf(), &stride, &offset);
		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_devcon->Draw(3, 0);

		window.flush();

	}


	void Scene::initShaders()
	{
		HRESULT result = D3DX11CompileFromFile((pathToShaders + L"vertexShader.hlsl").c_str(), 0, 0, "main", "vs_4_0", 0, 0, 0, &VSbinary, 0, 0);
		if (FAILED(result))
		{
			std::cerr << "Vertex shader was not initialized\n";
		}
		result = D3DX11CompileFromFile((pathToShaders + L"pixelShader.hlsl").c_str(), 0, 0, "main", "ps_4_0", 0, 0, 0, &PSbinary, 0, 0);
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
		constBufferDesc.ByteWidth = sizeof(ConstBuffer);
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
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
		//delete resources if it's already allocated

		D3D11_BUFFER_DESC bufferDescription;
		ZeroMemory(&bufferDescription, sizeof(bufferDescription));

		bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
		bufferDescription.ByteWidth = sizeof(Vertex) * vertices.size();
		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		g_device->CreateBuffer(&bufferDescription, NULL, vertexBuffer.GetAddressOf());

		//map (copy) resources
		D3D11_MAPPED_SUBRESOURCE mapSubresource;
		g_devcon->Map(vertexBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapSubresource);
		memcpy(mapSubresource.pData, vertices.data(), vertices.size() * sizeof(Vertex));
		g_devcon->Unmap(vertexBuffer.Get(), NULL);
	}


}
