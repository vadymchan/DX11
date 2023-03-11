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

		for (auto& modelInterseciton : modelIntersections)
		{
			if (modelInterseciton.triangleOctree.lock()->intersect(r, intersection, modelInterseciton.instance.lock()))
			{
				instance = modelInterseciton.instance;
				returnResult = true;
			}
		}

		return returnResult;

	}

	/// <param name="instances">instance may be change during dragging</param>
	void MeshSystem::updateOpaqueInstanceBuffer(const std::weak_ptr<Instance>& instance)
	{
		for (auto& modelIntersection : modelIntersections)
		{
			if (!modelIntersection.instance.expired() && !instance.expired() && modelIntersection.instance.lock() == instance.lock())
			{
				modelIntersection.opaqueInstance.lock()->needToUpdateInstanceBuffer();
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
				int triangleOctreeIndex = 0;
				for (; triangleOctreeIndex < triangleOctrees.size(); ++triangleOctreeIndex)
				{
					if (triangleOctrees[triangleOctreeIndex]->alreadyInited(model, meshIndices)) // already created
					{
						modelIntersections.emplace_back(ModelIntersection{ triangleOctrees.at(triangleOctreeIndex), instance, opaqueInstances.at(opaqueInstanceID) });
						break;
					}
				}

				if (triangleOctreeIndex == triangleOctrees.size())
				{
					triangleOctrees.emplace_back(std::make_shared<ModelTriangleOctree>(model, meshIndices));
					modelIntersections.emplace_back(ModelIntersection{ triangleOctrees.at(triangleOctrees.size() - 1), instance, opaqueInstances.at(opaqueInstanceID)});
				}
			}
		}


	}

	/// <summary>
	/// creates new Opaque Instance
	/// </summary>
	/// <returns>ID to access to created opaque instance</returns>
	uint32_t MeshSystem::createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& geometryShaderFileName)
	{
		std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
		opaqueInstance->setVertexShader(vertexShaderFileName);
		opaqueInstance->setPixelShader(pixelShaderFileName);
		if (!geometryShaderFileName.empty())
		{
			opaqueInstance->setGeometryShader(geometryShaderFileName);
		}
		opaqueInstances.push_back(opaqueInstance);
		return opaqueInstances.size() - 1;
	}

}