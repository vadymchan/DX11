#pragma once
#include "Mesh.h"
#include <cstdint>
#include <vector>
#include "../Buffer/IndexBuffer.h"
#include "../Buffer/VertexBuffer.h"


namespace engine::DX
{
	class Model
	{
	public:
		struct MeshRange
		{
			uint32_t vertexOffset;
			uint32_t indexOffset;
			uint32_t vertexNum;
			uint32_t indexNum;
		};


		UINT getMeshesCount()
		{
			return meshes.size();
		}

		void initVertexBuffer(UINT startSlot, const std::vector<UINT>& strides, const std::vector<UINT>& offsets, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0);

		void initIndexBuffer(D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0);



		/// <param name="inputSlot">index of slot in Input Assembler in which we bind vertex buffer</param>
		void bindBuffers(UINT inputSlot);

		
		const Mesh& getMesh(size_t index) const
		{
			return meshes.at(index);
		}

		Mesh& getMesh(size_t index)
		{
			return meshes.at(index);
		}


		const MeshRange& getMeshRange(size_t index)
		{
			return meshRanges.at(index);
		}

		
	protected:
		std::vector<Mesh> meshes;
		std::vector<MeshRange> meshRanges;
		VertexBuffer<Mesh::Vertex> vertices;
		IndexBuffer indices;

		friend class ModelManager;
	};
}
