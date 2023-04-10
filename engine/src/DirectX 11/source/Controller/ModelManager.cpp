#include "ModelManager.h"
#include <unordered_set>
namespace engine::DX
{
	const std::string ModelManager::cubeTag = "cube";
	const std::string ModelManager::debugCubeTag = "cubeDebug";
	const std::filesystem::path ModelManager::modelDirectory = "engine/src/general/resources/models/";

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
				dstMesh.vertices.at(v).position = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mVertices[v]);
				dstMesh.vertices.at(v).normal = reinterpret_cast<DirectX::SimpleMath::Vector3&>(srcMesh->mNormals[v]);
				dstMesh.vertices.at(v).texCoord = reinterpret_cast<DirectX::SimpleMath::Vector2&>(srcMesh->mTextureCoords[0][v]);
			}

			addTextureFiles(dstMesh, textureDesc, std::filesystem::path(modelFileName).parent_path() / "skins");




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
							TextureManager::getInstance().addTexture2D(fileEntry.path(), COLOR_TEXTURE_BIND_SLOT, textureDesc);

							mesh.textureFileNames[skinFolder].emplace_back(TextureManager::getInstance().getTexture2D(fileEntry.path()));
						}
					}
				}
			}
		}

	}


	void ModelManager::initUnitSphere()
	{
		//const uint32_t SIDES = 6;
		//const uint32_t GRID_SIZE = 12;
		//const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
		//const uint32_t VERT_PER_SIZE = (GRID_SIZE + 1) * (GRID_SIZE + 1);

		//Model model;

		////model.box = engine::Box::empty();

		//Mesh& mesh = model.meshes.emplace_back();
		//mesh.name = "UNIT_SPHERE";
		//mesh.box = Box::empty();
		//mesh.meshToModelMat = { float4x4::Identity};
		//mesh.invMeshToModelMat = { float4x4::Identity };

		//mesh.vertices.resize(VERT_PER_SIZE * SIDES);
		//Mesh::Vertex* vertex = mesh.vertices.data();

		//int sideMasks[6][3] =
		//{
		//	{ 2, 1, 0 },
		//	{ 0, 1, 2 },
		//	{ 2, 1, 0 },
		//	{ 0, 1, 2 },
		//	{ 0, 2, 1 },
		//	{ 0, 2, 1 }
		//};

		//float sideSigns[6][3] =
		//{
		//	{ +1, +1, +1 },
		//	{ -1, +1, +1 },
		//	{ -1, +1, -1 },
		//	{ +1, +1, -1 },
		//	{ +1, -1, -1 },
		//	{ +1, +1, +1 }
		//};

		//for (int side = 0; side < SIDES; ++side)
		//{
		//	for (int row = 0; row < GRID_SIZE + 1; ++row)
		//	{
		//		for (int col = 0; col < GRID_SIZE + 1; ++col)
		//		{
		//			float3 v;
		//			v.x = col / float(GRID_SIZE) * 2.f - 1.f;
		//			v.y = row / float(GRID_SIZE) * 2.f - 1.f;
		//			v.z = 1.f;

		//			vertex[0] = Mesh::Vertex::initial();

		//			vertex[0].position[sideMasks[side][0]] = v.x * sideSigns[side][0];
		//			vertex[0].position[sideMasks[side][1]] = v.y * sideSigns[side][1];
		//			vertex[0].position[sideMasks[side][2]] = v.z * sideSigns[side][2];
		//			vertex[0].normal = vertex[0].position = vertex[0].position.normalized();

		//			vertex += 1;
		//		}
		//	}
		//}

		//mesh.triangles.resize(TRIS_PER_SIDE * SIDES);
		//auto* triangle = mesh.triangles.data();

		//for (int side = 0; side < SIDES; ++side)
		//{
		//	uint32_t sideOffset = VERT_PER_SIZE * side;

		//	for (int row = 0; row < GRID_SIZE; ++row)
		//	{
		//		for (int col = 0; col < GRID_SIZE; ++col)
		//		{
		//			triangle[0].indices[0] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 0;
		//			triangle[0].indices[1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
		//			triangle[0].indices[2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

		//			triangle[1].indices[0] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 0;
		//			triangle[1].indices[1] = sideOffset + (row + 1) * (GRID_SIZE + 1) + col + 1;
		//			triangle[1].indices[2] = sideOffset + (row + 0) * (GRID_SIZE + 1) + col + 1;

		//			triangle += 2;
		//		}
		//	}
		//}

		//mesh.updateOctree();

		//addModel(DefaultModels[UNIT_SPHERE], std::move(model));
	}

	void ModelManager::initUnitSphereFlat()
	{
		/*const uint32_t SIDES = 6;
		const uint32_t GRID_SIZE = 12;
		const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
		const uint32_t VERT_PER_SIZE = 3 * TRIS_PER_SIDE;

		engine::Model model;
		model.name = "UNIT_SPHERE_FLAT";
		model.box = engine::Box::empty();

		engine::Mesh& mesh = model.meshes.emplace_back();
		mesh.name = "UNIT_SPHERE_FLAT";
		mesh.box = model.box;
		mesh.instances = { engine::Mat4::identity() };
		mesh.instancesInv = { engine::Mat4::identity() };

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

					vertex[0] = vertex[1] = vertex[2] = vertex[3] = Mesh::Vertex::initial();

					auto setPos = [sideMasks, sideSigns](int side, Mesh::Vertex& dst, const float3& pos)
					{
						dst.position[sideMasks[side][0]] = pos.x * sideSigns[side][0];
						dst.position[sideMasks[side][1]] = pos.y * sideSigns[side][1];
						dst.position[sideMasks[side][2]] = pos.z * sideSigns[side][2];
						dst.position = dst.position.normalized();
					};

					setPos(side, vertex[0], quad[0]);
					setPos(side, vertex[1], quad[1]);
					setPos(side, vertex[2], quad[2]);

					{
						float3 AB = vertex[1].position - vertex[0].position;
						float3 AC = vertex[2].position - vertex[0].position;
						vertex[0].normal = vertex[1].normal = vertex[2].normal = AB.cross(AC).normalized();
					}

					vertex += 3;

					setPos(side, vertex[0], quad[1]);
					setPos(side, vertex[1], quad[3]);
					setPos(side, vertex[2], quad[2]);

					{
						float3 AB = vertex[1].position - vertex[0].position;
						float3 AC = vertex[2].position - vertex[0].position;
						vertex[0].normal = vertex[1].normal = vertex[2].normal = AB.cross(AC).normalized();
					}

					vertex += 3;
				}
			}
		}

		mesh.updateOctree();

		addModel(DefaultModels[UNIT_SPHERE_FLAT], std::move(model));*/
	}




}