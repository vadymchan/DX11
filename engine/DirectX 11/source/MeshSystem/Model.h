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

		//Model(const std::vector<Mesh>& meshes, const std::vector<MeshRange>& meshRanges )
		//	: meshes{meshes}
		//	, meshRanges {meshRanges}
		//{

		//}

		//void addMesh(const Mesh& mesh, const MeshRange& meshRange)
		//{
		//	meshes.emplace_back(mesh);
		//	meshRanges.emplace_back(meshRange);
		//}

		UINT getMeshesCount()
		{
			return meshes.size();
		}

		void initVertexBuffer(UINT startSlot, const std::vector<UINT>& strides, const std::vector<UINT>& offsets, D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{

			
			std::vector<Mesh::Vertex> modelVertices{};
			for (size_t i = 0; i < meshes.size(); i++)
			{
				auto& meshVertices{ meshes.at(i).vertices };
				modelVertices.reserve(modelVertices.size() + meshVertices.size());
				modelVertices.insert(end(modelVertices), begin(meshVertices), end(meshVertices));
			}

			
			vertices.initBuffer(startSlot, strides, offsets, bufferUsage, modelVertices, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}

		void initIndexBuffer(D3D11_USAGE bufferUsage,
			UINT CPUAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0,
			UINT sysMemPitch = 0, UINT sysMemSlicePitch = 0)
		{
			//free indices
			//add indices

			std::vector<uint32_t> modelIndices{};
			for (size_t i = 0; i < meshes.size(); i++)
			{
				auto& meshIndices{ meshes.at(i).indices };
				modelIndices.reserve(modelIndices.size() + meshIndices.size());
				modelIndices.insert(end(modelIndices), begin(meshIndices), end(meshIndices));
			}
			indices.addElements(modelIndices);
			indices.initBuffer(bufferUsage, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
		}



		/// <param name="inputSlot">index of slot in Input Assembler in which we bind vertex buffer</param>
		void bindBuffers(UINT inputSlot)
		{
			vertices.setBuffer(inputSlot);
			indices.setBuffer();
		}


		
		//Mesh& operator[](size_t index) 
		//{
		//	return meshes.at(index);
		//}

		
		const Mesh& getMesh(size_t index) const
		{
			return meshes.at(index);
		}

		Mesh& getMesh(size_t index)
		{
			return meshes.at(index);
		}

		//const MeshRange& operator()(size_t index)
		//{
		//	return meshRanges.at(index);
		//}

		const MeshRange& getMeshRange(size_t index)
		{
			return meshRanges.at(index);
		}

		//void changeMeshSize(uint32_t newSize)
		//{
		//	meshes.resize(newSize);
		//}

		

		
	protected:
		std::vector<Mesh> meshes;
		std::vector<MeshRange> meshRanges;
		VertexBuffer<Mesh::Vertex> vertices;
		IndexBuffer indices;

		friend class ModelManager;
	};
}
