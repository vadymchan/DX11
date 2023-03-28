#include "ModelManager.h"
namespace engine::DX
{
	const std::string ModelManager::cubeTag = "cube";
	const std::string ModelManager::debugCubeTag = "cubeDebug";
	const std::string ModelManager::modelDirectory = "engine/src/general/resources/models/";

	void engine::DX::ModelManager::createModelAssimp(const std::string& modelFileName)
	{
		uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(modelDirectory + modelFileName, flags);

		assert(assimpScene != nullptr && ("model: " + modelFileName + " was not loaded").c_str());

		uint32_t numMeshes = assimpScene->mNumMeshes;



		Model model;

		model.meshes.resize(assimpScene->mNumMeshes);
		model.meshRanges.resize(assimpScene->mNumMeshes);


		static_assert(sizeof(float3) == sizeof(aiVector3D));

		for (uint32_t i = 0; i < numMeshes; ++i)
		{
			auto& srcMesh = assimpScene->mMeshes[i];
			auto& dstMesh = model.meshes.at(i);

			dstMesh.name = srcMesh->mName.C_Str();
			dstMesh.box.min = reinterpret_cast<float3&>(srcMesh->mAABB.mMin);
			dstMesh.box.max = reinterpret_cast<float3&>(srcMesh->mAABB.mMax);

			dstMesh.vertices.resize(srcMesh->mNumVertices);
			dstMesh.indices.resize(srcMesh->mNumFaces * 3);

			model.meshRanges.at(i).vertexNum = srcMesh->mNumVertices;
			model.meshRanges.at(i).indexNum = srcMesh->mNumFaces * 3;


			for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
			{
				dstMesh.vertices.at(v).position = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mVertices[v]);
				dstMesh.vertices.at(v).normal = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mNormals[v]);
				dstMesh.vertices.at(v).texCoord = reinterpret_cast<DirectX::SimpleMath::Vector2&>(srcMesh->mTextureCoords[0][v]);
			}


			for (uint32_t f = 0, index = 0; f < srcMesh->mNumFaces; ++f)
			{
				const auto& face = srcMesh->mFaces[f];
				assert(face.mNumIndices == 3);

				dstMesh.indices.at(index++) = face.mIndices[0];
				dstMesh.indices.at(index++) = face.mIndices[1];
				dstMesh.indices.at(index++) = face.mIndices[2];
			}

		}

		// Recursively load mesh instances (meshToModel transformation matrices)

		std::function<void(aiNode*)> loadInstances;
		loadInstances = [&loadInstances, &model](aiNode* node)
		{
			const float4x4 nodeToParent = reinterpret_cast<const float4x4&>(node->mTransformation);
			const float4x4 parentToNode = nodeToParent.Invert();

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
	}


	std::shared_ptr<Model> ModelManager::getModel(const std::string& modelFileName)
	{
		auto pair = models.find(modelFileName);
		if (pair != models.end()) // model is already created
		{
			return pair->second;
		}
		else if (modelFileName == cubeTag || modelFileName == debugCubeTag)
		{

			createCube(modelFileName);
		}
		else
		{
			createModelAssimp(modelFileName);
		}
		return models[modelFileName];
	}

	void engine::DX::ModelManager::createCube(const std::string& modelFileName)
	{
		Model cube;
		cube.meshes.resize(1);
		cube.meshRanges.resize(1);

		auto& mesh = cube.meshes.at(0);

		//cube data
		//----------------------------------------------------------------------

		//std::vector<Mesh::Vertex> cubeVerticesSeparateNormal = {
		//	{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f } }, // Front Face - Bottom Left
		//	{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f } }, // Front Face - Top Left
		//	{ { 0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f } }, // Front Face - Top Right
		//	{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f } }, // Front Face - Bottom Right

		//	{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f } }, // Back Face - Bottom Left
		//	{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f } }, // Back Face - Top Left
		//	{ { 0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f } }, // Back Face - Top Right
		//	{ { 0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f } }, // Back Face - Bottom Right

		//	{ { -0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f } }, // Bottom Face - Top Left
		//	{ { -0.5f, -0.5f,  0.5f },{ 0.0f, -1.0f, 0.0f } }, // Bottom Face - Bottom Left
		//	{ { 0.5f, -0.5f,  0.5f },{ 0.0f, -1.0f, 0.0f } }, // Bottom Face - Bottom Right
		//	{ { 0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f } }, // Bottom Face - Top Right

		//	{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } }, // Top Face - Top Left
		//	{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f } }, // Top Face - Bottom Left
		//	{ { 0.5f,  0.5f,  0.5f },{ 0.0f, 1.0f, 0.0f } }, // Top Face - Bottom Right
		//	{ { 0.5f,  0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } }, // Top Face - Top Right

		//	{ { 0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } }, // Right Face - Top Left
		//	{ { 0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f } }, // Right Face - Top Right
		//	{ { 0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } }, // Right Face - Bottom Left
		//	{ { 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f } }, // Right Face - Bottom Right

		//	{ { -0.5f,  0.5f,  0.5f },{ -1.0f, 0.0f, 0.0f } }, // Left Face - Top Left
		//	{ { -0.5f,  0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f } }, // Left Face - Top Right
		//	{ { -0.5f, -0.5f,  0.5f },{ -1.0f, 0.0f, 0.0f } }, // Left Face - Bottom Left
		//	{ { -0.5f, -0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f } }, // Left Face - Bottom Right


		//};

		std::vector<Mesh::Vertex> cubeVerticesSeparateNormal = {

			{ { -0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 1.0f } }, // Front Face - Bottom Left
			{ { -0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f },{ 0.0f, 0.0f } }, // Front Face - Top Left
			{ { 0.5f,  0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } }, // Front Face - Top Right
			{ { 0.5f, -0.5f, -0.5f },{ 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } }, // Front Face - Bottom Right

			{ { -0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } }, // Back Face - Bottom Left
			{ { -0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } }, // Back Face - Top Left
			{ { 0.5f,  0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }, // Back Face - Top Right
			{ { 0.5f, -0.5f,  0.5f },{ 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } }, // Back Face - Bottom Right

			{ { -0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, 0.0f } }, // Bottom Face - Top Left
			{ { -0.5f, -0.5f,  0.5f },{ 0.0f, -1.0f, 0.0f },{ 0.0f, 1.0f } }, // Bottom Face - Bottom Left
			{ { 0.5f, -0.5f,  0.5f },{ 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } }, // Bottom Face - Bottom Right
			{ { 0.5f, -0.5f, -0.5f },{ 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } }, // Bottom Face - Top Right

			{ { -0.5f,  0.5f, -0.5f},{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f }}, // Top Face - Top Left
			{ { -0.5f, 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }}, // Top Face - Bottom Left
			{ { 0.5f, 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f },  { 1.0f, 1.0f }}, // Top Face - Bottom Right
			{ { 0.5f, 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f }}, // Top Face - Top Right

			{ { 0.5f,  0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } }, // Right Face - Top Left
			{ { 0.5f,  0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } }, // Right Face - Top Right
			{ { 0.5f, -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f } }, // Right Face - Bottom Left
			{ { 0.5f, -0.5f,  0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } }, // Right Face - Bottom Right

			{ { -0.5f,  0.5f,  0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f }  }, // Left Face - Top Left
			{ { -0.5f,  0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f }  }, // Left Face - Top Right
			{ { -0.5f, -0.5f,  0.5f },{ -1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f }  }, // Left Face - Bottom Left
			{ { -0.5f, -0.5f, -0.5f },{ -1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f }  }, // Left Face - Bottom Right

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
		mesh.name = modelFileName;

		mesh.vertices = cubeVerticesSeparateNormal;
		mesh.indices = cubeIndicesSeparateNormal;
		mesh.meshToModelMat = { float4x4::Identity };
		mesh.invMeshToModelMat = { float4x4::Identity };

		//buffers
		cube.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
		cube.initIndexBuffer(D3D11_USAGE_DEFAULT);


		models[modelFileName] = std::make_shared<Model>(std::move(cube));


	}

}