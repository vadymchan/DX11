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

		Model()
		{

		}

		Model(const std::vector<Mesh>& meshes, const std::vector<MeshRange>& meshRanges )
			: meshes{meshes}
			, meshRanges {meshRanges}
		{

		}

		void addMesh(const Mesh& mesh, const MeshRange& meshRange)
		{
			meshes.emplace_back(mesh);
			meshRanges.emplace_back(meshRange);
			indices.addElements(mesh.getIndices());
		}

		UINT getMeshesCount()
		{
			return meshes.size();
		}

		void initVertexBuffer(UINT startSlot, UINT stride, UINT offset, D3D11_USAGE bufferUsage, const std::vector<Mesh::Vertex>& bufferData,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{

			UINT vertexNum{};
			for (auto& meshRange : meshRanges)
			{
				vertexNum += meshRange.vertexNum;
			}

			//several strides and offsets
			// pData

			vertices.initBuffer(startSlot, stride, offset, bufferUsage, bufferData, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void initIndexBuffer(D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			//free indices
			//add indices

			indices.initBuffer(bufferUsage, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}



		/// <param name="inputSlot">index of slot in Input Assembler in which we bind vertex buffer</param>
		void bindBuffers(UINT inputSlot)
		{
			vertices.setBuffer(inputSlot);
			indices.setBuffer();
		}

		const Mesh& operator[](size_t index) // getMesh(size_t meshIndex)
		{
			return meshes.at(index);
		}

		const Mesh& getMesh(size_t index)
		{
			return meshes.at(index);
		}

		const MeshRange& operator()(size_t index)
		{
			return meshRanges.at(index);
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

	};
}
