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

		const static std::string cubeTag;
		const static std::string debugCubeTag;
		const static std::string modelDirectory;

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

	};
}
