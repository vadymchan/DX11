#pragma once
#include "../D3D/D3D.h"
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
			const static std::vector<UINT> alignedByteOffsets;
		};
		//bounding box
		//name
		//triangles - Mesh::Triangle
		//vertices - Mesh::Vertex
		//instances - Mat4 (from mesh to model space)
		//instances Inv - Mat4

		Mesh()
		{
		}

		Mesh(const std::vector<Vertex> vertices, const std::vector<uint32_t>& indices)
			: vertices{vertices}
			, indices{indices}
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


	private:
		std::vector<uint32_t> indices{};
		std::vector<Vertex> vertices{}; 
		std::string name;
		friend class Model;
		friend class ModelManager;
	};

	
}

