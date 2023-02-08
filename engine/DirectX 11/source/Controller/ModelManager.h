#pragma once
#include "../MeshSystem/Model.h"
#include "BufferManager.h"
#include <string>
#include <unordered_map>

namespace engine::DX
{
	class ModelManager
	{
	public:

		static ModelManager& getInstance()
		{
			static ModelManager instance;
			return instance;
		}

		/*Model loadAssimp(std::string pathToModel)
		{
			
			return Model();
		}*/

		std::shared_ptr<Model> loadCube()
		{
			auto pair = models.find(cubeTag);
			if (pair != models.end()) // cube is already created
			{
				return pair->second;
			}
			createCube();
			return models[cubeTag];
		}

		const std::string cubeTag = "cube";

	protected:
		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		void createCube()
		{
			Model cube;

			//mesh
			std::vector<Mesh> meshes;
			Mesh mesh;

			//std::vector<Mesh::Vertex> vertices
			//{
			//	Mesh::Vertex{{-0.5f,0.5f,  0.0f, 1.0f} , {1,0,0,0} },
			//	Mesh::Vertex{{-0.5f,-0.5f, 0.0f, 1.0f} , {0,1,0,0} },
			//	Mesh::Vertex{{0.5f,-0.5f,  0.0f, 1.0f} , {0,0,1,0} }
			//};
			/*std::vector<Mesh::Vertex> vertices
			{
				Mesh::Vertex{{-0.5f,0.5f,  0.0f, 1.0f}},
				Mesh::Vertex{{-0.5f,-0.5f, 0.0f, 1.0f}},
				Mesh::Vertex{{0.5f,-0.5f,  0.0f, 1.0f}}
			};
			std::vector<uint32_t> indices{ 0, 2, 1 };*/
			//mesh.addVertices(vertices);
			//mesh.addIndices(indices);
			//meshes.emplace_back(mesh);

			//

			////mesh range
			//std::vector<Model::MeshRange> meshRanges;
			//Model::MeshRange meshRange;
			//meshRange.vertexOffset = 0;
			//meshRange.indexOffset = 0;
			//meshRange.indexNum = indices.size();
			//meshRange.vertexNum = vertices.size();
			//meshRanges.emplace_back(meshRange);

			//cube.addMesh(mesh, meshRange);

			////buffers
			//cube.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, sizeof(Mesh::Vertex), 0, D3D11_USAGE_IMMUTABLE, vertices);
			//cube.initIndexBuffer(D3D11_USAGE_DEFAULT);

			


			//cube data
			//----------------------------------------------------------------------
			std::vector<Mesh::Vertex> cubeVertices
			{
				Mesh::Vertex{{-0.5f,  0.5f, -0.5f, 1.0f}},  // upper left  front
				Mesh::Vertex{{-0.5f, -0.5f, -0.5f, 1.0f}},  // lower left  front
				Mesh::Vertex{{ 0.5f, -0.5f, -0.5f, 1.0f}},  // lower right front
				Mesh::Vertex{{ 0.5f,  0.5f, -0.5f, 1.0f}},  // upper right front
				Mesh::Vertex{{-0.5f,  0.5f,  0.5f, 1.0f}},  // upper left  back
				Mesh::Vertex{{-0.5f, -0.5f,  0.5f, 1.0f}},	// lower left  back
				Mesh::Vertex{{ 0.5f, -0.5f,  0.5f, 1.0f}},	// lower right back
				Mesh::Vertex{{ 0.5f,  0.5f,  0.5f, 1.0f}},	// upper right back

			};

			//std::vector<uint32_t> cubeIndices
			//{
			//	0, 3, 1,
			//	1, 3, 2,

			//	4, 7, 5,
			//	5, 7, 6,

			//	2, 7, 3,
			//	2, 6, 7,

			//	0, 4, 1,
			//	1, 4, 5,

			//	0, 3, 4,
			//	3, 7, 4,

			//	1, 5, 2,
			//	2, 5, 6,
			//};


			std::vector<uint32_t> cubeIndices
			{
				0, 1, 3,
				1, 2, 3,
				4, 5, 7,
				5, 6, 7,
				2, 3, 7,
				2, 7, 6,
				0, 1, 4,
				1, 5, 4,
				0, 4, 3,
				3, 4, 7,
				1, 2, 5,
				2, 6, 5,
			};

			mesh.addVertices(cubeVertices);
			mesh.addIndices( cubeIndices);
			meshes.emplace_back(mesh);



			//mesh range
			std::vector<Model::MeshRange> meshRanges;
			Model::MeshRange meshRange;
			meshRange.vertexOffset = 0;
			meshRange.indexOffset = 0;
			meshRange.indexNum = cubeIndices.size();
			meshRange.vertexNum = cubeVertices.size();
			meshRanges.emplace_back(meshRange);

			cube.addMesh(mesh, meshRange);

			//buffers
			cube.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, sizeof(Mesh::Vertex), 0, D3D11_USAGE_IMMUTABLE, cubeVertices);
			cube.initIndexBuffer(D3D11_USAGE_DEFAULT);


			models[cubeTag] = std::make_shared<Model>(std::move(cube));

			//----------------------------------------------------------------------

		}
	};
}
