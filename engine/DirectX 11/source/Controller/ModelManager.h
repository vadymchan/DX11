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

		std::shared_ptr<Model> loadAssimp(std::string modelFileName)
		{
			uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded |aiProcess_CalcTangentSpace);

			Assimp::Importer importer;
			const aiScene* assimpScene = importer.ReadFile(modelDirectory + modelFileName, flags);

			if (assimpScene == nullptr)
			{
				std::cerr << "model: " + modelFileName + " was not loaded" << std::endl;
			}

			uint32_t numMeshes = assimpScene->mNumMeshes;

			// Load vertex data

			Model model;
			//model.name = path;
			//model.box = {};
			//std::vector<Mesh> meshes(assimpScene->mNumMeshes);
			//std::vector<Model::MeshRange> meshRanges(assimpScene->mNumMeshes);

			model.meshes.resize(assimpScene->mNumMeshes);
			model.meshRanges.resize(assimpScene->mNumMeshes);


			static_assert(sizeof(DirectX::SimpleMath::Vector3) == sizeof(aiVector3D));
			//static_assert(sizeof(Mesh::Triangle) == 3 * sizeof(uint32_t));

			for (uint32_t i = 0; i < numMeshes; ++i)
			{
				auto& srcMesh = assimpScene->mMeshes[i];
				auto& dstMesh = model.meshes.at(i);



				dstMesh.name = srcMesh->mName.C_Str();
				dstMesh.box.min = reinterpret_cast<float3&>(srcMesh->mAABB.mMin);
				dstMesh.box.max = reinterpret_cast<float3&>(srcMesh->mAABB.mMax);



				dstMesh.vertices.resize(srcMesh->mNumVertices);
				dstMesh.indices.resize(srcMesh->mNumFaces * 3);
				//at(i).vertices.resize(srcMesh->mNumVertices);
			//dstMesh.triangles.resize(srcMesh->mNumFaces);

			//std::vector<Mesh::Vertex> meshVertices(srcMesh->mNumVertices);


				model.meshRanges.at(i).vertexNum = srcMesh->mNumVertices;
				model.meshRanges.at(i).indexNum = srcMesh->mNumFaces * 3;


				for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
				{
					//DirectX::SimpleMath::Vector3 t = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mVertices[v]);
					//dstMesh.vertices.at(v).position = float4(t.x, t.y, t.z, 1);
					dstMesh.vertices.at(v).position = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mVertices[v]);
					dstMesh.vertices.at(v).normal = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mNormals[v]);


					//vertex.tc = reinterpret_cast<engine::Vec2f&>(srcMesh->mTextureCoords[0][v]);
					//vertex.tangent = reinterpret_cast<engine::float3&>(srcMesh->mTangents[v]);
					//vertex.bitangent = reinterpret_cast<engine::float3&>(srcMesh->mBitangents[v]) * -1.f; // Flip V
				}

				//meshes.at(i).addVertices(meshVertices);

				for (uint32_t f = 0, index = 0; f < srcMesh->mNumFaces; ++f)
				{
					const auto& face = srcMesh->mFaces[f];
					assert(face.mNumIndices == 3);

					dstMesh.indices.at(index++) = face.mIndices[0];
					dstMesh.indices.at(index++) = face.mIndices[1];
					dstMesh.indices.at(index++) = face.mIndices[2];
				}

				//dstMesh.updateOctree();
			}

			// Recursively load mesh instances (meshToModel transformation matrices)
			
			std::function<void(aiNode*)> loadInstances;
			loadInstances = [&loadInstances, &model](aiNode* node)
			{
				const float4x4 nodeToParent = reinterpret_cast<const float4x4&>(node->mTransformation);
				const float4x4 parentToNode = nodeToParent.Invert();

				/*const float4x4 nodeToParent = DirectX::SimpleMath::Matrix::Identity;
				const float4x4 parentToNode = DirectX::SimpleMath::Matrix::Identity;*/

				// The same node may contain multiple meshes in its space, referring to them by indices
				for (uint32_t i = 0; i < node->mNumMeshes; ++i)
				{
					
					uint32_t meshIndex = node->mMeshes[i];
					model.meshes[meshIndex].meshToModelMat.push_back(nodeToParent);
					model.meshes[meshIndex].invMeshToModelMat.push_back(parentToNode);
				}

				for (uint32_t i = 0; i < node->mNumChildren; ++i)
					loadInstances(node->mChildren[i]);
			};

			loadInstances(assimpScene->mRootNode);




			model.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
			model.initIndexBuffer(D3D11_USAGE_DEFAULT);


			models[modelFileName] = std::make_shared<Model>(std::move(model));

			return models[modelFileName];
		}

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

		void createCube()
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


			std::vector<Mesh::Vertex> cubeVertices
			{
				Mesh::Vertex{{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},  // upper left  front
				Mesh::Vertex{{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},  // lower left  front
				Mesh::Vertex{{ 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},  // lower right front
				Mesh::Vertex{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}},  // upper right front
				Mesh::Vertex{{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},  // upper left  back
				Mesh::Vertex{{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},	// lower left  back
				Mesh::Vertex{{ 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},	// lower right back
				Mesh::Vertex{{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},	// upper right back

			};

			std::vector<uint32_t> cubeIndices
			{
				//front
				0, 3, 1,
				1, 3, 2,
				////back 
				4, 5, 7,
				5, 6, 7,
				//right
				2, 3, 7,
				2, 7, 6,
				//left
				0, 1, 4,
				1, 5, 4,
				//up
				0, 4, 3,
				3, 4, 7,
				//down
				1, 2, 5,
				2, 6, 5,
			};

			//mesh range
			auto& meshRange = cube.meshRanges.at(0);
			meshRange.vertexOffset = 0;
			meshRange.indexOffset = 0;
			meshRange.indexNum = cubeIndicesSeparateNormal.size();
			meshRange.vertexNum = cubeVerticesSeparateNormal.size();
			mesh.box.min = { -0.5f, -0.5f, -0.5f };
			mesh.box.max = { 0.5f, 0.5f, 0.5f };
			mesh.name = "cube";

			mesh.vertices = cubeVerticesSeparateNormal;
			mesh.indices = cubeIndicesSeparateNormal;
			mesh.meshToModelMat = { float4x4::Identity };
	/*		mesh.meshToModelMat = { engine::DX::float4x4
					{{{1,0,0,5},
					{0,1,0,-5},
					{0,0,1,0},
					{0,0,0,1},}} };*/
			mesh.invMeshToModelMat = { float4x4::Identity };

			//buffers
			cube.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
			cube.initIndexBuffer(D3D11_USAGE_DEFAULT);


			models[cubeTag] = std::make_shared<Model>(std::move(cube));

			//----------------------------------------------------------------------

		}
	};
}
