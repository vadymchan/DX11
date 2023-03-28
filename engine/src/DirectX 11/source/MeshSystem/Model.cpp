#include "Model.h"
namespace engine::DX
{

	void Model::initVertexBuffer(UINT startSlot, const std::vector<UINT>& strides, const std::vector<UINT>& offsets, D3D11_USAGE bufferUsage, UINT CPUAccessFlags, UINT miscFlags, UINT structureByteStride, UINT sysMemPitch, UINT sysMemSlicePitch)
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

	void Model::initIndexBuffer(D3D11_USAGE bufferUsage, UINT CPUAccessFlags, UINT miscFlags, UINT structureByteStride, UINT sysMemPitch, UINT sysMemSlicePitch)
	{
		std::vector<uint32_t> modelIndices{};
		for (size_t i = 0; i < meshes.size(); i++)
		{
			auto& meshIndices{ meshes.at(i).indices };
			modelIndices.reserve(modelIndices.size() + meshIndices.size());
			modelIndices.insert(end(modelIndices), begin(meshIndices), end(meshIndices));
		}
		indices.setBufferData(modelIndices);
		indices.initBuffer(bufferUsage, CPUAccessFlags, miscFlags, structureByteStride, sysMemPitch, sysMemSlicePitch);
	}

	/// <param name="inputSlot">index of slot in Input Assembler in which we bind vertex buffer</param>
	void Model::bindBuffers(UINT inputSlot)
	{
		vertices.setBuffer(inputSlot);
		indices.setBuffer();
	}

}