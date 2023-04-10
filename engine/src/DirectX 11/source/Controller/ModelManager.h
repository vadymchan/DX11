#pragma once
#include "../MeshSystem/Model.h"
#include "BufferManager.h"
#include "TextureManager.h"
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>


namespace engine::DX
{

	using float4 = DirectX::SimpleMath::Vector4;
	class ModelManager
	{
	public:

		const static std::string cubeTag;
		const static std::string debugCubeTag;
		const static std::filesystem::path modelDirectory;

		static ModelManager& getInstance()
		{
			static ModelManager instance;
			return instance;
		}

		
		std::shared_ptr<Model> getModel(const std::string& modelFileName);

	protected:

		std::unordered_map<std::string, std::shared_ptr<Model>> models;

		void createModelAssimp(const std::string& modelFileName);

		void createCube(const std::string& modelFileName);

		void addTextureFiles(Mesh& mesh, const D3D11_TEXTURE2D_DESC& textureDesc, const std::filesystem::path& texturePath);

		void initUnitSphereFlat();

		void initUnitSphere();

	};
}
