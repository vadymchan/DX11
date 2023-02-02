#pragma once

#include "D3D.h"
#include "../source/Vertex.h"
#include "../source/ConstBuffer.h"
#include "../window/WindowDX.h"
#include <vector>
#include "../../general/utils/timer/FPSTimerRC.h"
#include <algorithm>

namespace engine::DX
{
	class Scene
	{
	public:
		void render(Window& window);
		void initPipeline();
		void initVertexBuffer();
		void addVertex(const Vertex& vertex) { vertices.emplace_back(vertex); }

	private:
		engine::general::FPSTimer timer;
		void initShaders();
		void initInputLayout();
		void initConstantBuffer();
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11PixelShader> pixelShader;
		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11InputLayout> inputLayout;
		ComPtr<ID3DBlob> VSbinary;
		ComPtr<ID3DBlob> PSbinary;
		ComPtr<ID3D11Buffer> constBuffer; 
		ConstBuffer constBufferData;
		UINT stride{ sizeof(Vertex) };
		UINT offset{};
		std::vector<Vertex> vertices;

	};
}



