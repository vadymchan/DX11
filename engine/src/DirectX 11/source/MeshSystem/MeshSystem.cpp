#include "MeshSystem.h"


namespace engine::DX
{

	/// <param name="instance">instance, which is contained in instanceGroup</param>
	bool MeshSystem::findIntersection(const ray& r, TransformSystem::ID& instance, RayIntersection& intersection)
	{
		bool returnResult{};

		for (auto& modelInterseciton : modelIntersections)
		{
			if (modelInterseciton.triangleOctree.lock()->intersect(r, intersection, modelInterseciton.instance))
			{
				instance = modelInterseciton.instance;
				returnResult = true;
			}
		}

		return returnResult;

	}

	/// <param name="instances">instance may be change during dragging</param>
	void MeshSystem::updateInstanceBuffer(const TransformSystem::ID instance)
	{
		for (auto& modelIntersection : modelIntersections)
		{
			if (modelIntersection.instance == instance)
			{
				modelIntersection.instanceGroup.lock()->needToUpdateInstanceBuffer();
			}
		}
	}

	void MeshSystem::addOpaqueInstances(OpaqueInstancesID opaqueInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<OpaqueInstances::Instance>& transformID)
	{
		bool debugCubeModel{};

		std::vector<OpaqueInstances::Instance> instances{};
		instances.resize(transformID.size());
		for (size_t i = 0; i < transformID.size(); i++)
		{
			instances[i] = OpaqueInstances::Instance{ transformID[i] };
		}

		for (size_t i = 0; i < meshIndices.size(); ++i)
		{

			opaqueInstances.at(opaqueInstanceID)->addInstances(model, meshIndices[i], material, instances);

			if (model->getMesh(i).GetName() == ModelManager::debugCubeTag)
			{
				debugCubeModel = true;
			}

		}

		for (auto& instanceID : transformID)
		{
			if (!debugCubeModel)
			{
				int triangleOctreeIndex = 0;
				for (; triangleOctreeIndex < triangleOctrees.size(); ++triangleOctreeIndex)
				{
					if (triangleOctrees[triangleOctreeIndex]->alreadyInited(model, meshIndices)) // already created
					{
						modelIntersections.emplace_back(
							ModelIntersection{ triangleOctrees[triangleOctreeIndex], instanceID.worldMatrixID, opaqueInstances[opaqueInstanceID] });
						break;
					}
				}

				if (triangleOctreeIndex == triangleOctrees.size())
				{
					triangleOctrees.emplace_back(std::make_shared<ModelTriangleOctree>(model, meshIndices));
					modelIntersections.emplace_back(
						ModelIntersection{ triangleOctrees[triangleOctrees.size() - 1], instanceID.worldMatrixID, opaqueInstances[opaqueInstanceID] });
				}
			}
		}


	}


	void MeshSystem::addDissolutionInstances(DissolutionInstancesID dissolutionInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices, const std::shared_ptr<DissolutionInstances::Material>& material, const std::vector<std::shared_ptr<DissolutionInstances::Instance>>& instances)
	{
		bool debugCubeModel{};

		for (size_t i = 0; i < meshIndices.size(); ++i)
		{

			dissolutionInstances.at(dissolutionInstanceID)->addInstances(model, meshIndices[i], material, instances);

			if (model->getMesh(i).GetName() == ModelManager::debugCubeTag)
			{
				debugCubeModel = true;
			}

		}

		for (auto& instanceID : instances)
		{
			if (!debugCubeModel)
			{
				int triangleOctreeIndex = 0;
				for (; triangleOctreeIndex < triangleOctrees.size(); ++triangleOctreeIndex)
				{
					if (triangleOctrees[triangleOctreeIndex]->alreadyInited(model, meshIndices)) // already created
					{
						modelIntersections.emplace_back(
							ModelIntersection{ triangleOctrees[triangleOctreeIndex], instanceID->worldMatrixID, dissolutionInstances[dissolutionInstanceID] });
						break;
					}
				}

				if (triangleOctreeIndex == triangleOctrees.size())
				{
					triangleOctrees.emplace_back(std::make_shared<ModelTriangleOctree>(model, meshIndices));
					modelIntersections.emplace_back(
						ModelIntersection{ triangleOctrees[triangleOctrees.size() - 1], instanceID->worldMatrixID, dissolutionInstances[dissolutionInstanceID] });
				}
			}
		}


	}

	/// <summary>
	/// creates new Opaque Instance
	/// </summary>
	/// <returns>ID to access to created opaque instance</returns>
	MeshSystem::OpaqueInstancesID MeshSystem::createOpaqueInstance(const std::vector<std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames)
	{
		std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
		opaqueInstance->setShaders(shaderFileNames);
		opaqueInstances.emplace_back(opaqueInstance);
		return (opaqueInstances.size() - 1);
	}

	/// <summary>
	/// creates new Dissolution Instance
	/// </summary>
	/// <returns>ID to access to created opaque instance</returns>
	MeshSystem::DissolutionInstancesID MeshSystem::createDissolutionInstance(const std::vector<std::array<std::wstring, (int)DissolutionInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames)
	{
		std::shared_ptr<DissolutionInstances> dissolutionInstance = std::make_shared<DissolutionInstances>();
		dissolutionInstance->setShaders(shaderFileNames);
		dissolutionInstances.emplace_back(dissolutionInstance);
		return (dissolutionInstances.size() - 1);
	}

	MeshSystem::RenderMode MeshSystem::getRenderMode(const BaseInstances::ShaderGroup& shaderGroup)
	{

		switch (shaderGroup.type)
		{
		case BaseInstances::RenderType::NORMAL_VISUALIZER:
			return RenderMode::NORMAL_VISUALISER;
		case BaseInstances::RenderType::HOLOGRAM:
			return RenderMode::HOLOGRAM;
		case BaseInstances::RenderType::DEFAULT:
			return RenderMode::DEFAULT;
		case BaseInstances::RenderType::BLINN_PHONG:
			return RenderMode::BLINN_PHONG;
		case BaseInstances::RenderType::POINT_SPHERE:
			return RenderMode::POINT_SPHERE;
		case BaseInstances::RenderType::PBR:
			return RenderMode::PBR;
		case BaseInstances::RenderType::IBL_SHADOW:
			return RenderMode::IBL_SHADOW;
		case BaseInstances::RenderType::IBL:
			return RenderMode::IBL;
		default:
			return RenderMode::DEFAULT;
		}


	}

	void MeshSystem::setShaders(const BaseInstances::ShaderGroup& shaderGroup)
	{
		//vertex shader
		shaderGroup.vertexShader.lock()->bind();

		//hull shader
		if (!shaderGroup.hullShader.expired())
		{
			shaderGroup.hullShader.lock()->bind();
		}
		else
		{
			g_devcon->HSSetShader(nullptr, nullptr, 0);
		}

		//domain shader
		if (!shaderGroup.domainShader.expired())
		{
			shaderGroup.domainShader.lock()->bind();
		}
		else
		{
			g_devcon->DSSetShader(nullptr, nullptr, 0);
		}

		//geometry shader
		if (!shaderGroup.geometryShader.expired())
		{
			shaderGroup.geometryShader.lock()->bind();
		}
		else
		{
			g_devcon->GSSetShader(nullptr, nullptr, 0);
		}

		//pixel shader
		shaderGroup.pixelShader.lock()->bind();


		//set additional resources for the shader
		for (auto& shaderResource : shaderGroup.shaderResources)
		{
			if (!shaderResource.expired())
			{
				shaderResource.lock()->bind();
			}
		}

	}

	void MeshSystem::setPrimitiveTopology(const BaseInstances::ShaderGroup& shaderGroup)
	{
		//topology
		if (!shaderGroup.hullShader.expired() && !shaderGroup.domainShader.expired())
		{
			g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		}
		else
		{
			g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}



}