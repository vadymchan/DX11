#pragma once
#include "../D3D/D3D.h"
#include "../Math/Colision/boxDX.h"
#include <vector>

namespace engine::DX
{
	class Mesh
	{
	public:
		struct Vertex
		{
			DirectX::SimpleMath::Vector3 position;
			DirectX::SimpleMath::Vector3 normal;
			DirectX::SimpleMath::Vector2 texCoord;
			const static std::vector<UINT> alignedByteOffsets;
		};


		Mesh()
		{
		}

		Mesh(const std::vector<Vertex> vertices, const std::vector<uint32_t>& indices)
			: vertices{ vertices }
			, indices{ indices }
		{

		}

		const std::vector<uint32_t>& getIndices() const
		{
			return indices;
		}

		const std::vector<Vertex>& getVertices() const
		{
			return vertices;
		}



		void addVertices(const std::vector<Vertex>& newVertices)
		{
			vertices.reserve(vertices.size() + newVertices.size());
			vertices.insert(end(vertices), begin(newVertices), end(newVertices));
		}

		void addIndices(const std::vector<uint32_t>& newIndices)
		{
			indices.reserve(indices.size() + newIndices.size());
			indices.insert(end(indices), begin(newIndices), end(newIndices));
		}

		const std::vector<float4x4>& getMeshToModelMat() const
		{
			return meshToModelMat;
		}

		const float4x4& getMeshToModelMat(uint32_t index) const
		{
			assert(index < meshToModelMat.size() && "getMeshToModelMat in Mesh: out of bounds. Error!");
			return meshToModelMat.at(index);

		}

		const std::vector<float4x4>& getInvMeshToModelMat() const
		{
			return invMeshToModelMat;
		}

		const float4x4& getInvMeshToModelMat(uint32_t index) const
		{
			assert(index < invMeshToModelMat.size() && "getInvMeshToModelMat in Mesh: out of bounds. Error!");
			return invMeshToModelMat.at(index);

		}


		const std::string& GetName() const
		{
			return name;
		}

		const Box& getBox() const { return box; }


	private:
		std::vector<uint32_t> indices{};
		std::vector<Vertex> vertices{};
		std::vector<float4x4> meshToModelMat;
		std::vector<float4x4> invMeshToModelMat;
		std::string name;
		Box box;
		friend class Model;
		friend class ModelManager;
	};


}

