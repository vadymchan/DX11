#include "MeshSystem.h"


namespace engine::DX
{




	void MeshSystem::render(const DirectX::SimpleMath::Vector3& cameraPos)
	{
		for (const auto& opaqueInstance : opaqueInstances)
		{
			opaqueInstance->render(cameraPos);
		}
	}

	/// <param name="instance">instance, which is contained in opaqueInstance</param>
	bool MeshSystem::findIntersection(const ray& r, std::weak_ptr<Instance>& instance, Intersection& intersection)
	{
		bool returnResult{};

		for (auto& triangleOctree : triangleOctrees)
		{
			if (triangleOctree->intersect(r, intersection))
			{
				instance = triangleOctree->getInstance();
				returnResult = true;
			}
		}

		return returnResult;

	}

	/// <param name="instances">instance may be change during dragging</param>
	void MeshSystem::updateOpaqueInstanceBuffer(const std::weak_ptr<Instance>& instance)
	{
		for (auto& triangleOctreeInstance : triangleOctreeInstances)
		{
			if (!triangleOctreeInstance.first.expired() && !instance.expired() &&triangleOctreeInstance.first.lock() == instance.lock())
			{
				triangleOctreeInstance.second->needToUpdateInstanceBuffer();
			}
		}
	}

	void MeshSystem::addInstances(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<std::shared_ptr<OpaqueInstances::Instance>>& instances)
	{ 
		bool debugCubeModel{};
		for (size_t meshIndex = 0; meshIndex < meshIndices.size(); ++meshIndex)
		{

		opaqueInstances.at(opaqueInstanceID)->addInstances(model, meshIndices.at(meshIndex), material, instances);
		
		if (model->getMesh(meshIndex).GetName() == ModelManager::debugCubeTag)
		{
			debugCubeModel = true;
		}

		}

		for (auto& instance : instances)
		{

			if (!debugCubeModel)
			{
				triangleOctrees.emplace_back(std::make_shared<ModelTriangleOctree>(model, std::weak_ptr<Instance>(instance), meshIndices));
				triangleOctreeInstances.emplace_back(std::make_pair(instance, opaqueInstances.at(opaqueInstanceID).get()));
			}


		}


	}

	/// <summary>
	/// creates new Opaque Instance
	/// </summary>
	/// <returns>ID to access to created opaque instance</returns>
	uint32_t MeshSystem::createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName)
	{
		std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
		opaqueInstance->setVertexShader(vertexShaderFileName);
		opaqueInstance->setPixelShader(pixelShaderFileName);
		opaqueInstances.push_back(opaqueInstance);
		return opaqueInstances.size() - 1;
	}

}