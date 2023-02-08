#pragma once

#include "../source/D3D/D3D.h"
#include "../source/VertexType.h"
#include "../source/ConstBufferType.h"
#include "../window/WindowDX.h"
#include "../../general/utils/timer/FPSTimerRC.h"
#include <vector>
#include <algorithm>

namespace engine::DX
{
	class Scene
	{
	public:
		class Color
		{
		public:
			DirectX::XMFLOAT4 color;
		};

		class Position
		{
		public:
			DirectX::XMFLOAT4 pos;
			
		};
		std::vector<Color> colors;
		std::vector<Position> positions;

		void render(Window& window);
		void initPipeline();
		void initVertexBuffer();
		void addVertex(const VertexType& vertex) { vertices.emplace_back(vertex);
		
		
		
		}

	private:
		engine::general::FPSTimer timer;
		void initShaders();
		void initInputLayout();
		void initConstantBuffer();
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3D11PixelShader> pixelShader;
		std::vector<ComPtr<ID3D11Buffer>> vertexBuffer;
		ComPtr<ID3D11InputLayout> inputLayout;
		ComPtr<ID3DBlob> VSbinary;
		ComPtr<ID3DBlob> PSbinary;
		ComPtr<ID3D11Buffer> constBuffer; 
		ConstBufferType constBufferData;
		UINT stride{ sizeof(VertexType) };
		UINT offset{};
		std::vector<VertexType> vertices;


	};
}



