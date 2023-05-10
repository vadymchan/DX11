#include "ModelManager.h"
#include <unordered_set>


namespace engine::DX
{
	const std::string ModelManager::cubeTag = "cube";
	const std::string ModelManager::debugCubeTag = "CubeDebug";
	const std::string ModelManager::sphereTag = "sphere";
	const std::string ModelManager::flatSphereTag = "flatSphere";
	const std::string ModelManager::floorTag = "floor";
	const std::filesystem::path ModelManager::modelDirectory = "engine/src/general/resources/models/";

	constexpr float PI = 3.14159265358979323846;

	void calculateTangentBitangent(const Mesh::Vertex& v0, const Mesh::Vertex& v1, const Mesh::Vertex& v2, float3& tangent, float3& bitangent)
	{
		float3 edge1 = v1.position - v0.position;
		float3 edge2 = v2.position - v0.position;
		float2 deltaUV1 = v1.texCoord - v0.texCoord;
		float2 deltaUV2 = v2.texCoord - v0.texCoord;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent.Normalize();

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent.Normalize();
	}

	void ModelManager::addTextureFiles(Mesh& mesh, const D3D11_TEXTURE2D_DESC& textureDesc, const std::filesystem::path& texturePath)
	{
		mesh.textureLocation = modelDirectory / texturePath;

		// iterate through directories in the textureLocation
		for (const auto& directoryEntry : std::filesystem::directory_iterator(mesh.textureLocation))
		{
			if (directoryEntry.is_directory())
			{
				const std::wstring& skinFolder = directoryEntry.path().filename().wstring();

				// iterate through files in the current directory
				for (const auto& fileEntry : std::filesystem::directory_iterator(directoryEntry.path()))
				{
					if (fileEntry.is_regular_file() && fileEntry.path().extension() == ".dds")
					{
						std::string fileName = fileEntry.path().filename().string();
						// find the textures that have the same name as the mesh name (and exclude the _mesh part)
						if (fileName.find(mesh.name.substr(0, mesh.name.find("_mesh"))) != std::string::npos)
						{
							// add the file to the textureFileNames map
							uint32_t textureBindSlot;

							if (fileName.find("albedo") != std::string::npos)
								textureBindSlot = ALBEDO_TEXTURE_BIND_SLOT;
							else if (fileName.find("metallic") != std::string::npos)
								textureBindSlot = METALNESS_TEXTURE_BIND_SLOT;
							else if (fileName.find("roughness") != std::string::npos)
								textureBindSlot = ROUGHNESS_TEXTURE_BIND_SLOT;
							else if (fileName.find("normal") != std::string::npos)
								textureBindSlot = NORMAL_TEXTURE_BIND_SLOT;
							else
								textureBindSlot = COLOR_TEXTURE_BIND_SLOT;

							TextureManager::getInstance().addTexture2D(fileEntry.path(), textureBindSlot, textureDesc);

							mesh.textureFileNames[skinFolder].emplace_back(TextureManager::getInstance().getTexture2D(fileEntry.path()));
						}
					}
				}
			}
		}
		mesh.textureFileNames[L""] = {};

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
		else if (modelFileName == sphereTag)
		{
			createUnitSphere();
		}
		else if (modelFileName == flatSphereTag)
		{
			createUnitSphereFlat();
		}
		else if (modelFileName == floorTag)
		{
			createFloor(modelFileName);
		}
		else
		{
			createModelAssimp(modelFileName);
		}
		return models[modelFileName];
	}

	void engine::DX::ModelManager::createModelAssimp(const std::string& modelFileName)
	{
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;


		uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(modelDirectory.string() + modelFileName, flags);

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
				dstMesh.vertices.at(v).position = reinterpret_cast<float3&>(srcMesh->mVertices[v]);
				dstMesh.vertices.at(v).normal = reinterpret_cast<float3&>(srcMesh->mNormals[v]);
				dstMesh.vertices.at(v).texCoord = reinterpret_cast<float2&>(srcMesh->mTextureCoords[0][v]);
			}

			addTextureFiles(dstMesh, textureDesc, std::filesystem::path(modelFileName).parent_path() / "skins");

			std::vector<float3> tangents(srcMesh->mNumVertices, float3::Zero);
			std::vector<float3> bitangents(srcMesh->mNumVertices, float3::Zero);



			for (uint32_t f = 0, index = 0; f < srcMesh->mNumFaces; ++f)
			{
				const auto& face = srcMesh->mFaces[f];
				assert(face.mNumIndices == 3);

				dstMesh.indices.at(index++) = face.mIndices[0];
				dstMesh.indices.at(index++) = face.mIndices[1];
				dstMesh.indices.at(index++) = face.mIndices[2];
			}

			for (uint32_t f = 0; f < srcMesh->mNumFaces; ++f)
			{
				const auto& face = srcMesh->mFaces[f];
				assert(face.mNumIndices == 3);

				// Calculate the tangent and bitangent for the face
				float3 vertex0 = float3(srcMesh->mVertices[face.mIndices[0]].x, srcMesh->mVertices[face.mIndices[0]].y, srcMesh->mVertices[face.mIndices[0]].z);
				float3 vertex1 = float3(srcMesh->mVertices[face.mIndices[1]].x, srcMesh->mVertices[face.mIndices[1]].y, srcMesh->mVertices[face.mIndices[1]].z);
				float3 vertex2 = float3(srcMesh->mVertices[face.mIndices[2]].x, srcMesh->mVertices[face.mIndices[2]].y, srcMesh->mVertices[face.mIndices[2]].z);

				float2 uv0 = float2(srcMesh->mTextureCoords[0][face.mIndices[0]].x, srcMesh->mTextureCoords[0][face.mIndices[0]].y);
				float2 uv1 = float2(srcMesh->mTextureCoords[0][face.mIndices[1]].x, srcMesh->mTextureCoords[0][face.mIndices[1]].y);
				float2 uv2 = float2(srcMesh->mTextureCoords[0][face.mIndices[2]].x, srcMesh->mTextureCoords[0][face.mIndices[2]].y);


				float3 edge1 = vertex1 - vertex0;
				float3 edge2 = vertex2 - vertex0;
				float2 deltaUV1 = uv1 - uv0;
				float2 deltaUV2 = uv2 - uv0;

				float uvFactor = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
				float3 tangent;
				float3 bitangent;

				tangent.x = uvFactor * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = uvFactor * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = uvFactor * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

				bitangent.x = uvFactor * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
				bitangent.y = uvFactor * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
				bitangent.z = uvFactor * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

				// Accumulate the tangent and bitangent for each vertex
				for (int v = 0; v < 3; ++v)
				{
					tangents[face.mIndices[v]] += tangent;
					bitangents[face.mIndices[v]] += bitangent;
				}
			}

			// Normalize and orthogonalize the tangent and bitangent for each vertex
			for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
			{
				dstMesh.vertices.at(v).position = reinterpret_cast<float3&>(srcMesh->mVertices[v]);
				dstMesh.vertices.at(v).normal = reinterpret_cast<float3&>(srcMesh->mNormals[v]);
				dstMesh.vertices.at(v).texCoord = reinterpret_cast<float2&>(srcMesh->mTextureCoords[0][v]);
				float3& T = tangents[v];
				float3& B = bitangents[v];
				float3 N = dstMesh.vertices.at(v).normal;

				// Gram-Schmidt orthogonalize
				T = (T - N * N.Dot(T));
				T.Normalize();

				// Calculate handedness (to support mirrored normal maps)
				float handedness = (N.Cross(T).Dot(B) < 0.0f) ? -1.0f : 1.0f;
				B = N.Cross(T) * handedness;

				dstMesh.vertices.at(v).tangent = T;
				dstMesh.vertices.at(v).bitangent = B;
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

	//void engine::DX::ModelManager::createModelAssimp(const std::string& modelFileName)
	//{
	//	D3D11_TEXTURE2D_DESC textureDesc{};
	//	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	//	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//	textureDesc.CPUAccessFlags = 0;
	//	textureDesc.MiscFlags = 0;

	//	uint32_t flags = uint32_t(aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	//	Assimp::Importer importer;
	//	const aiScene* assimpScene = importer.ReadFile(modelDirectory.string() + modelFileName, flags);

	//	assert(assimpScene != nullptr && ("model: " + modelFileName + " was not loaded").c_str());

	//	uint32_t numMeshes = assimpScene->mNumMeshes;

	//	Model model;

	//	model.meshes.resize(assimpScene->mNumMeshes);
	//	model.meshRanges.resize(assimpScene->mNumMeshes);

	//	static_assert(sizeof(float3) == sizeof(aiVector3D));

	//	for (uint32_t i = 0; i < numMeshes; ++i)
	//	{
	//		auto& srcMesh = assimpScene->mMeshes[i];
	//		auto& dstMesh = model.meshes.at(i);

	//		dstMesh.name = srcMesh->mName.C_Str();
	//		dstMesh.box.min = reinterpret_cast<float3&>(srcMesh->mAABB.mMin);
	//		dstMesh.box.max = reinterpret_cast<float3&>(srcMesh->mAABB.mMax);

	//		dstMesh.vertices.resize(srcMesh->mNumVertices);
	//		dstMesh.indices.resize(srcMesh->mNumFaces * 3);

	//		model.meshRanges.at(i).vertexNum = srcMesh->mNumVertices;
	//		model.meshRanges.at(i).indexNum = srcMesh->mNumFaces * 3;

	//		for (uint32_t v = 0; v < srcMesh->mNumVertices; ++v)
	//		{
	//			dstMesh.vertices.at(v).position = reinterpret_cast<float3&>(srcMesh->mVertices[v]);
	//			dstMesh.vertices.at(v).normal = reinterpret_cast<float3&>(srcMesh->mNormals[v]);
	//			dstMesh.vertices.at(v).texCoord = reinterpret_cast<float2&>(srcMesh->mTextureCoords[0][v]);
	//			dstMesh.vertices.at(v).tangent = reinterpret_cast<float3&>(srcMesh->mTangents[v]);
	//			dstMesh.vertices.at(v).bitangent = reinterpret_cast<float3&>(srcMesh->mBitangents[v]);
	//		}

	//		addTextureFiles(dstMesh, textureDesc, std::filesystem::path(modelFileName).parent_path() / "skins");

	//		for (uint32_t f = 0, index = 0; f < srcMesh->mNumFaces; ++f)
	//		{
	//			const auto& face = srcMesh->mFaces[f];
	//			assert(face.mNumIndices == 3);

	//			dstMesh.indices.at(index++) = face.mIndices[0];
	//			dstMesh.indices.at(index++) = face.mIndices[1];
	//			dstMesh.indices.at(index++) = face.mIndices[2];
	//		}
	//	}

	//	std::function<void(aiNode*)> loadInstances;
	//	loadInstances = [&loadInstances, &model](aiNode* node)
	//	{
	//		const float4x4 nodeToParent = reinterpret_cast<const float4x4&>(node->mTransformation);
	//		const float4x4 parentToNode = nodeToParent.Invert();
	//		// The same node may contain multiple meshes in its space, referring to them by indices
	//		for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	//		{
	//			uint32_t meshIndex = node->mMeshes[i];
	//			model.meshes[meshIndex].meshToModelMat.push_back(nodeToParent);
	//			model.meshes[meshIndex].invMeshToModelMat.push_back(parentToNode);
	//		}

	//		for (uint32_t i = 0; i < node->mNumChildren; ++i)
	//			loadInstances(node->mChildren[i]);
	//	};

	//	loadInstances(assimpScene->mRootNode);

	//	model.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
	//	model.initIndexBuffer(D3D11_USAGE_DEFAULT);

	//	models[modelFileName] = std::make_shared<Model>(std::move(model));
	//}


	void engine::DX::ModelManager::createCube(const std::string& modelFileName)
	{
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

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

		//calculate tangent and bitangent
		for (size_t i = 0; i < cubeIndicesSeparateNormal.size(); i += 3)
		{
			Mesh::Vertex& v0 = cubeVerticesSeparateNormal[cubeIndicesSeparateNormal[i]];
			Mesh::Vertex& v1 = cubeVerticesSeparateNormal[cubeIndicesSeparateNormal[i + 1]];
			Mesh::Vertex& v2 = cubeVerticesSeparateNormal[cubeIndicesSeparateNormal[i + 2]];

			float3 tangent, bitangent;
			calculateTangentBitangent(v0, v1, v2, tangent, bitangent);

			v0.tangent = tangent;
			v1.tangent = tangent;
			v2.tangent = tangent;

			v0.bitangent = bitangent;
			v1.bitangent = bitangent;
			v2.bitangent = bitangent;
		}

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


		addTextureFiles(mesh, textureDesc, std::filesystem::path("Cube/skins"));




		//buffers
		cube.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
		cube.initIndexBuffer(D3D11_USAGE_DEFAULT);


		models[modelFileName] = std::make_shared<Model>(std::move(cube));


	}



	void ModelManager::createUnitSphere()
	{
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		const uint32_t SIDES = 6;
		const uint32_t GRID_SIZE = 12;
		const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
		const uint32_t VERT_PER_SIZE = (GRID_SIZE + 1) * (GRID_SIZE + 1);

		Model model;

		Mesh& mesh = model.meshes.emplace_back();
		auto& meshRange = model.meshRanges.emplace_back();
		mesh.name = sphereTag;
		mesh.box = Box::empty();
		mesh.meshToModelMat = { float4x4::Identity };
		mesh.invMeshToModelMat = { float4x4::Identity };

		mesh.vertices.resize(VERT_PER_SIZE * SIDES);
		Mesh::Vertex* vertex = mesh.vertices.data();

		int sideMasks[6][3] =
		{
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 0, 2, 1 },
			{ 0, 2, 1 }
		};

		float sideSigns[6][3] =
		{
			{ +1, +1, +1 },
			{ -1, +1, +1 },
			{ -1, +1, -1 },
			{ +1, +1, -1 },
			{ +1, -1, -1 },
			{ +1, +1, +1 }
		};

		for (int side = 0; side < SIDES; ++side)
		{
			for (int row = 0; row < GRID_SIZE + 1; ++row)
			{
				for (int col = 0; col < GRID_SIZE + 1; ++col)
				{
					float3 v;
					v.x = col / float(GRID_SIZE) * 2.f - 1.f;
					v.y = row / float(GRID_SIZE) * 2.f - 1.f;
					v.z = 1.f;

					//vertex[0] = Mesh::Vertex::initial();
					vertex[0] = Mesh::Vertex{};

					// Calculate the values for x, y, and z based on pos and sideSigns.
					const float values[3] = { v.x * sideSigns[side][0], v.y * sideSigns[side][1], v.z * sideSigns[side][2] };

					// Iterate over the sideMasks[side] array.
					for (int i = 0; i < 3; i++) {
						int index = sideMasks[side][i];

						// Assign the appropriate value to vertex[0].position.x, 
						// vertex[0].position.y, or vertex[0].position.z based on the value of sideMasks[side][i].
						switch (index) {
						case 0:
							vertex[0].position.x = values[i];
							break;
						case 1:
							vertex[0].position.y = values[i];
							break;
						case 2:
							vertex[0].position.z = values[i];
							break;
						}
					}
					float3 normal = vertex[0].position;
					normal.Normalize();
					vertex[0].normal = vertex[0].position = normal;


					// Compute tangent and bitangent
					float3 T = { -normal.y, normal.x, 0.0f };
					T.Normalize();
					vertex[0].tangent = T;
					vertex[0].bitangent = vertex[0].normal.Cross(vertex[0].tangent);

					// texcoord (not checked yet)
					vertex[0].texCoord.x = float(col) / float(GRID_SIZE);
					vertex[0].texCoord.y = float(row) / float(GRID_SIZE);

					vertex += 1;
				}
			}
		}


		mesh.indices.resize(TRIS_PER_SIDE * SIDES * 3);
		auto* indices = mesh.indices.data();

		size_t index = 0;

		for (int side = 0; side < SIDES; ++side)
		{
			uint32_t sideOffset = VERT_PER_SIZE * side;

			for (int row = 0; row < GRID_SIZE; ++row)
			{
				for (int col = 0; col < GRID_SIZE; ++col)
				{
					indices[index++] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 0;
					indices[index++] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
					indices[index++] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

					indices[index++] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
					indices[index++] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 1;
					indices[index++] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;
				}
			}
		}

		//mesh.updateOctree(); // we don't need it because the sphere is unit
		mesh.box.min = float3(-1, -1, -1);
		mesh.box.max = float3(1, 1, 1);

		meshRange.vertexOffset = 0;
		meshRange.indexOffset = 0;
		meshRange.indexNum = mesh.indices.size();
		meshRange.vertexNum = mesh.vertices.size();

		addTextureFiles(mesh, textureDesc, "Sphere/skins");

		model.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
		model.initIndexBuffer(D3D11_USAGE_DEFAULT);

		models[mesh.name] = std::make_shared<Model>(std::move(model));
		//addModel(DefaultModels[UNIT_SPHERE], std::move(model));
	}

	void ModelManager::createUnitSphereFlat()
	{
		const uint32_t SIDES = 6;
		const uint32_t GRID_SIZE = 12;
		const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
		const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

		Model model;

		Mesh& mesh = model.meshes.emplace_back();
		auto& meshRange = model.meshRanges.emplace_back();
		mesh.name = flatSphereTag;
		mesh.meshToModelMat = { float4x4::Identity };
		mesh.invMeshToModelMat = { float4x4::Identity };

		mesh.vertices.resize(VERT_PER_SIZE * SIDES);
		Mesh::Vertex* vertex = mesh.vertices.data();

		int sideMasks[6][3] =
		{
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 2, 1, 0 },
			{ 0, 1, 2 },
			{ 0, 2, 1 },
			{ 0, 2, 1 }
		};

		float sideSigns[6][3] =
		{
			{ +1, +1, +1 },
			{ -1, +1, +1 },
			{ -1, +1, -1 },
			{ +1, +1, -1 },
			{ +1, -1, -1 },
			{ +1, +1, +1 }
		};

		for (int side = 0; side < SIDES; ++side)
		{
			for (int row = 0; row < GRID_SIZE; ++row)
			{
				for (int col = 0; col < GRID_SIZE; ++col)
				{
					float left = (col + 0) / float(GRID_SIZE) * 2.f - 1.f;
					float right = (col + 1) / float(GRID_SIZE) * 2.f - 1.f;
					float bottom = (row + 0) / float(GRID_SIZE) * 2.f - 1.f;
					float top = (row + 1) / float(GRID_SIZE) * 2.f - 1.f;

					float3 quad[4] =
					{
						{ left, bottom, 1.f },
						{ left, top, 1.f },
						{ right, bottom, 1.f },
						{ right, top, 1.f }
					};

					vertex[0] = vertex[1] = vertex[2] = vertex[3] = Mesh::Vertex{};

					auto setPos = [sideMasks, sideSigns](int side, Mesh::Vertex& dst, const float3& pos)
					{

						// Calculate the values for x, y, and z based on pos and sideSigns.
						const float values[3] = { pos.x * sideSigns[side][0], pos.y * sideSigns[side][1], pos.z * sideSigns[side][2] };

						// Iterate over the sideMasks[side] array.
						for (int i = 0; i < 3; i++) {
							int index = sideMasks[side][i];

							// Assign the appropriate value to dst.position.x,
							// dst.position.y, or dst.position.z based on the value of sideMasks[side][i].
							switch (index) {
							case 0:
								dst.position.x = values[i];
								break;
							case 1:
								dst.position.y = values[i];
								break;
							case 2:
								dst.position.z = values[i];
								break;
							}
						}

						dst.position.Normalize();
					};

					setPos(side, vertex[0], quad[0]);
					setPos(side, vertex[1], quad[1]);
					setPos(side, vertex[2], quad[2]);

					auto computeTexCoords = [](const float3& position) -> float2
					{
						float theta = atan2(position.z, position.x) + PI;
						float phi = acos(position.y);

						float u = theta / (2 * PI);
						float v = phi / PI;

						return { u, v };
					};

					{
						float3 AB = vertex[1].position - vertex[0].position;
						float3 AC = vertex[2].position - vertex[0].position;
						float3 normal = AB.Cross(AC);
						normal.Normalize();
						vertex[0].normal = vertex[1].normal = vertex[2].normal = normal;

						// Compute tangent and bitangent
						float3 T = { -normal.y, normal.x, 0.0f };
						T.Normalize();
						vertex[0].tangent = vertex[1].tangent = vertex[2].tangent = T;
						vertex[0].bitangent = vertex[1].bitangent = vertex[2].bitangent = normal.Cross(T);

						// not tested yet
						vertex[0].texCoord = computeTexCoords(vertex[0].position);
						vertex[1].texCoord = computeTexCoords(vertex[1].position);
						vertex[2].texCoord = computeTexCoords(vertex[2].position);

					}

					vertex += 3;

					setPos(side, vertex[0], quad[1]);
					setPos(side, vertex[1], quad[3]);
					setPos(side, vertex[2], quad[2]);

					{
						float3 AB = vertex[1].position - vertex[0].position;
						float3 AC = vertex[2].position - vertex[0].position;
						float3 normal = AB.Cross(AC);
						normal.Normalize();
						vertex[0].normal = vertex[1].normal = vertex[2].normal = normal;

						// Compute tangent and bitangent
						float3 T = { -normal.y, normal.x, 0.0f };
						T.Normalize();
						vertex[0].tangent = vertex[1].tangent = vertex[2].tangent = T;
						vertex[0].bitangent = vertex[1].bitangent = vertex[2].bitangent = normal.Cross(T);

						// not tested yet
						vertex[0].texCoord = computeTexCoords(vertex[0].position);
						vertex[1].texCoord = computeTexCoords(vertex[1].position);
						vertex[2].texCoord = computeTexCoords(vertex[2].position);

					}

					vertex += 3;
				}
			}
		}


		mesh.indices.resize(TRIS_PER_SIDE * SIDES * 3);
		auto* indices = mesh.indices.data();

		size_t index = 0;

		for (int side = 0; side < SIDES; ++side)
		{
			uint32_t sideOffset = VERT_PER_SIZE * side;

			for (int row = 0; row < GRID_SIZE; ++row)
			{
				for (int col = 0; col < GRID_SIZE; ++col)
				{
					uint32_t quad_offset = sideOffset + (row * GRID_SIZE + col) * 6;

					indices[index++] = quad_offset + 0;
					indices[index++] = quad_offset + 1;
					indices[index++] = quad_offset + 2;

					indices[index++] = quad_offset + 3;
					indices[index++] = quad_offset + 4;
					indices[index++] = quad_offset + 5;
				}
			}
		}


		//mesh.updateOctree(); // we don't need it because the sphere is unit
		mesh.box.min = float3(-1, -1, -1);
		mesh.box.max = float3(1, 1, 1);


		meshRange.vertexOffset = 0;
		meshRange.indexOffset = 0;
		meshRange.indexNum = mesh.indices.size();
		meshRange.vertexNum = mesh.vertices.size();

		model.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
		model.initIndexBuffer(D3D11_USAGE_DEFAULT);

		models[mesh.name] = std::make_shared<Model>(std::move(model));

	}

	void ModelManager::createFloor(const std::string& modelFileName)
	{
		D3D11_TEXTURE2D_DESC textureDesc{};
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		Model floor;
		floor.meshes.resize(1);
		floor.meshRanges.resize(1);

		auto& mesh = floor.meshes[0];
		auto& meshRange = floor.meshRanges[0];
		mesh.name = floorTag;
		mesh.meshToModelMat = { float4x4::Identity };
		mesh.invMeshToModelMat = { float4x4::Identity };

		std::vector<Mesh::Vertex> floorVertices = {
			{{-0.5f, 0.0f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.0f, -0.5f},  {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.0f, 0.5f},   {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{-0.5f, 0.0f, 0.5f},  {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f},  {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
		};

		std::vector<unsigned int> floorIndices = {
			0, 2, 1,
			0, 3, 2
		};

		//bounding box
		mesh.box.min = { -0.5, -0.001, -0.5 };
		mesh.box.max = { 0.5, 0.001, 0.5 };

		//mesh data (vertices)
		mesh.vertices = std::move(floorVertices);
		mesh.indices = std::move(floorIndices);

		//textures
		addTextureFiles(mesh, textureDesc, std::filesystem::path("Floor/skins"));

		//mesh range
		meshRange.vertexOffset = 0;
		meshRange.indexOffset = 0;
		meshRange.indexNum = mesh.indices.size();
		meshRange.vertexNum = mesh.vertices.size();

		//vertex and index buffer
		floor.initVertexBuffer(MODEL_DATA_INPUT_SLOT_0, std::vector<UINT>{sizeof(Mesh::Vertex)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE);
		floor.initIndexBuffer(D3D11_USAGE_DEFAULT);

		models[modelFileName] = std::make_shared<Model>(std::move(floor));
	}



}