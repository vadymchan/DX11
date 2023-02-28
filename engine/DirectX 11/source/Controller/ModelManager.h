#pragma once
#include "../MeshSystem/Model.h"
#include "BufferManager.h"
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace engine::DX
{

	using float4 = DirectX::SimpleMath::Vector4;
	class ModelManager
	{
	public:

		const std::string cubeTag = "cube";
		const std::string modelDirectory = "engine/general/resources/models/";

		static ModelManager& getInstance()
		{
			static ModelManager instance;
			return instance;
		}

		std::shared_ptr<Model> loadAssimp(std::string modelFileName);

		std::shared_ptr<Model> loadCube();

		std::shared_ptr<Model> loadCube1()
		{
			auto pair = models.find("cube1");
			if (pair != models.end()) // cube is already created
			{
				return pair->second;
			}
			createCube1();
			return models["cube1"];
		}

		void createCube1()
		{
			Model cube;
			cube.meshes.resize(1);
			cube.meshRanges.resize(1);

			auto& mesh = cube.meshes.at(0);

			//cube data
			//----------------------------------------------------------------------

			std::vector<Mesh::Vertex> cubeVerticesSeparateNormal = {
				{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f} }, // Front Face - Bottom Left
				{ {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f} }, // Front Face - Top Left
				{ { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f} }, // Front Face - Top Right
				{ { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f} }, // Front Face - Bottom Right

				{ {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f} }, // Back Face - Bottom Left
				{ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f} }, // Back Face - Top Left
				{ { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f} }, // Back Face - Top Right
				{ { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f} }, // Back Face - Bottom Right

				{ {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f} }, // Bottom Face - Top Left
				{ {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f} }, // Bottom Face - Bottom Left
				{ { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f} }, // Bottom Face - Bottom Right
				{ { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f} }, // Bottom Face - Top Right

				{ {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} }, // Top Face - Top Left
				{ {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f} }, // Top Face - Bottom Left
				{ { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f} }, // Top Face - Bottom Right
				{ { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f} }, // Top Face - Top Right

				{ { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} }, // Right Face - Top Left
				{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f} }, // Right Face - Top Right
				{ { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f} }, // Right Face - Bottom Left
				{ { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f} }, // Right Face - Bottom Right

				{ {-0.5f,  0.5f,  0.5f }, {-1.0f, 0.0f, 0.0f} }, // Left Face - Top Left
				{ {-0.5f,  0.5f, -0.5f }, {-1.0f, 0.0f, 0.0f} }, // Left Face - Top Right
				{ {-0.5f, -0.5f,  0.5f }, {-1.0f, 0.0f, 0.0f} }, // Left Face - Bottom Left
				{ {-0.5f, -0.5f, -0.5f }, {-1.0f, 0.0f, 0.0f} }, // Left Face - Bottom Right


			};

			std::vector<unsigned int> cubeIndicesSeparateNormal = {
				0, 1, 2,
				2, 3, 0, // Front Face

				4, 6, 5,
				6, 4, 7, // Back Face

				8,  10, 9,
				10, 8,  11, // Bottom Face

				12, 13, 14,
				14, 15, 12, // Top Face

				16, 17, 18,
				18, 17, 19, // Right Face

				20, 21, 22,
				22, 21, 23, // left face
			};



			//mesh range
			auto& meshRange = cube.meshRanges.at(0);
			meshRange.vertexOffset = 0;
			meshRange.indexOffset = 0;
			meshRange.indexNum = cubeIndicesSeparateNormal.size();
			meshRange.vertexNum = cubeVerticesSeparateNormal.size();
			mesh.box.min = { -0.5f, -0.5f, -0.5f };
			mesh.box.max = { 0.5f, 0.5f, 0.5f };
			mesh.name = "cube1";

			mesh.vertices = cubeVerticesSeparateNormal;
			mesh.indices = cubeIndicesSeparateNormal;
			mesh.meshToModelMat = { float4x4::Identity };
			mesh.invMeshToModelMat = { float4x4::Identity };

			//buffers
			cube.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
			cube.initIndexBuffer(D3D11_USAGE_DEFAULT);


			models["cube1"] = std::make_shared<Model>(std::move(cube));

			//----------------------------------------------------------------------

		}


	protected:

		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		void createCube();
	};
}
