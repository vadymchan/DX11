#pragma once
#include "../D3D/D3D.h"
#include "../Buffer/VertexBuffer.h"
#include "../Controller/TextureManager.h"
#include "../Controller/BufferManager.h"
#include "Texture2D.h"
#include <array>
#include <vector>

namespace engine::DX
{
	class Skybox
	{
	public:
		struct Vertex
		{
			float3 position;
			float2 texCoord;
			const static std::vector<UINT> alignedByteOffsets;
		};



		void initSkybox(const std::wstring& textureFileName)
		{
			m_textureFileName = textureFileName;
			static bool isInitialized = false;
			if (!isInitialized)
			{
				m_vertexBuffer.initBuffer(SKYBOX_VERTEX_BUFFER_DATA_0, { sizeof(Vertex) }, { 0 }, D3D11_USAGE_IMMUTABLE, std::vector<Vertex>(m_vertices.begin(), m_vertices.end()));
				isInitialized = true;
			}
		}

		void render()
		{
			//m_vertexBuffer.setBuffer();
			
		}

		void setTexture(const std::wstring& textureFileName)
		{
			m_textureFileName = textureFileName;
		}

	private:

		//rendering using a fullscreen triangle
		const static std::array<Vertex, 3> m_vertices;
		static VertexBuffer<Vertex> m_vertexBuffer; // static since m_vertices is the same for all objects of Skybox

		std::wstring m_textureFileName;

	};


	

}
