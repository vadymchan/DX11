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
	class ModelManager
	{
	public:

		static ModelManager& getInstance()
		{
			static ModelManager instance;
			return instance;
		}

		std::shared_ptr<Model> loadAssimp(std::string pathToModel)
		{
			
			uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

			Assimp::Importer importer;
			const aiScene* assimpScene = importer.ReadFile(pathToModel, flags);
			//DEV_ASSERT(assimpScene);

			uint32_t numMeshes = assimpScene->mNumMeshes;

			// Load vertex data

			Model model;
			//model.name = path;
			//model.box = {};
			//model.meshes.resize(numMeshes);

			//static_assert(sizeof(Vec3f) == sizeof(aiVector3D));
			//static_assert(sizeof(Mesh::Triangle) == 3 * sizeof(uint32_t));

			//for (uint32_t i = 0; i < numMeshes; ++i)
			//{
			//	auto& srcMesh = assimpScene->mMeshes[i];
			//	auto& dstMesh = model.meshes[i];

			//	dstMesh.name = srcMesh->mName.C_Str();
			//	dstMesh.box.min = reinterpret_cast<Vec3f&>(srcMesh->mAABB.mMin);
			//	dstMesh.box.max = reinterpret_cast<Vec3f&>(srcMesh->mAABB.mMax);

			//	dstMesh.vertices.resize(srcMesh->mNumVertices);
			//	dstMesh.triangles.resize(srcMesh->mNumFaces);

			//	for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
			//	{
			//		Mesh::Vertex& vertex = dstMesh.vertices[v];
			//		vertex.pos = reinterpret_cast<engine::Vec3f&>(srcMesh->mVertices[v]);
			//		vertex.tc = reinterpret_cast<engine::Vec2f&>(srcMesh->mTextureCoords[0][v]);
			//		vertex.normal = reinterpret_cast<engine::Vec3f&>(srcMesh->mNormals[v]);
			//		vertex.tangent = reinterpret_cast<engine::Vec3f&>(srcMesh->mTangents[v]);
			//		vertex.bitangent = reinterpret_cast<engine::Vec3f&>(srcMesh->mBitangents[v]) * -1.f; // Flip V
			//	}

			//	for (uint32_t f = 0; f < srcMesh->mNumFaces; ++f)
			//	{
			//		const auto& face = srcMesh->mFaces[f];
			//		DEV_ASSERT(face.mNumIndices == 3);
			//		dstMesh.triangles[f] = *reinterpret_cast<Mesh::Triangle*>(face.mIndices);
			//	}

			//	dstMesh.updateOctree();
			//}

			//// Recursively load mesh instances (meshToModel transformation matrices)

			//std::function<void(aiNode*)> loadInstances;
			//loadInstances = [&loadInstances, &model](aiNode* node)
			//{
			//	const float4x4 nodeToParent = reinterpret_cast<const float4x4&>(node->mTransformation.Transpose());
			//	const float4x4 parentToNode = nodeToParent.Invert();;

			//	// The same node may contain multiple meshes in its space, referring to them by indices
			//	for (uint32_t i = 0; i < node->mNumMeshes; ++i)
			//	{
			//		uint32_t meshIndex = node->mMeshes[i];
			//		model.meshes[meshIndex].instances.push_back(nodeToParent);
			//		model.meshes[meshIndex].instancesInv.push_back(parentToNode);
			//	}

			//	for (uint32_t i = 0; i < node->mNumChildren; ++i)
			//		loadInstances(node->mChildren[i]);
			//};

			//loadInstances(assimpScene->mRootNode);


			return nullptr;


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

		const std::string cubeTag = "cube";

	protected:
		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		void createCube()
		{
			Model cube;

			//mesh
			std::vector<Mesh> meshes;
			Mesh mesh;



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
