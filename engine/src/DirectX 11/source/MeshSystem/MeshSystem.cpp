#include "MeshSystem.h"


namespace engine::DX
{




	void MeshSystem::render(Camera& camera, bool showNormal)
	{
		for (const auto& opaqueInstance : opaqueInstances)
		{
			//bind shaders (draw several times with different shader)
			for (auto& shaderGroup : opaqueInstance->getShaders())
			{
				RenderMode renderModeType = getRenderMode(shaderGroup);

				switch (renderModeType)
				{
				case RenderMode::DEFAULT:
					SetRenderMode<RenderMode::DEFAULT>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::NORMAL_VISUALISER:
					if (!showNormal)
						continue;
					SetRenderMode<RenderMode::NORMAL_VISUALISER>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::HOLOGRAM:
					SetRenderMode<RenderMode::HOLOGRAM>(opaqueInstance.get(), &camera);
					break;

				}

				setShaders(shaderGroup);
				setPrimitiveTopology(shaderGroup);


				opaqueInstance->render();
			}
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

			opaqueInstances.at(opaqueInstanceID)->addInstances(model, meshIndices[meshIndex], material, instances);

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
						modelIntersections.emplace_back(ModelIntersection{ triangleOctrees[triangleOctreeIndex], instance, opaqueInstances[opaqueInstanceID] });
						break;
					}
				}

				if (triangleOctreeIndex == triangleOctrees.size())
				{
					triangleOctrees.emplace_back(std::make_shared<ModelTriangleOctree>(model, meshIndices));
					modelIntersections.emplace_back(ModelIntersection{ triangleOctrees[triangleOctrees.size() - 1], instance, opaqueInstances[opaqueInstanceID] });
				}
			}
		}


	}

	/// <summary>
	/// creates new Opaque Instance
	/// </summary>
	/// <returns>ID to access to created opaque instance</returns>
	uint32_t MeshSystem::createOpaqueInstance(const std::vector<std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>>& shaderFileNames)
	{
		std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
		opaqueInstance->setShaders(shaderFileNames);
		opaqueInstances.push_back(opaqueInstance);
		return (opaqueInstances.size() - 1);
	}

	MeshSystem::RenderMode MeshSystem::getRenderMode(const OpaqueInstances::ShaderGroup& shaderGroup)
	{

		switch (shaderGroup.type)
		{
		case OpaqueInstances::RenderType::NORMAL_VISUALIZER:
			return RenderMode::NORMAL_VISUALISER;
		case OpaqueInstances::RenderType::HOLOGRAM:
			return RenderMode::HOLOGRAM;
		case OpaqueInstances::RenderType::DEFAULT:
			return RenderMode::DEFAULT;
		default:
			return RenderMode::DEFAULT;
		}


	}

	void MeshSystem::setShaders(const OpaqueInstances::ShaderGroup& shaderGroup)
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
	}

	void MeshSystem::setPrimitiveTopology(const OpaqueInstances::ShaderGroup& shaderGroup)
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