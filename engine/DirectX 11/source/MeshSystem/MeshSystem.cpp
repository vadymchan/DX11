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
					renderMode<RenderMode::DEFAULT>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::NORMAL_VISUALISER:
					if (!showNormal)
						continue;
					renderMode<RenderMode::NORMAL_VISUALISER>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::HOLOGRAM:
					renderMode<RenderMode::HOLOGRAM>(opaqueInstance.get(), &camera);
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
					modelIntersections.emplace_back(ModelIntersection{ triangleOctrees.at(triangleOctrees.size() - 1), instance, opaqueInstances.at(opaqueInstanceID) });
				}
			}
		}


	}

	/// <summary>
	/// creates new Opaque Instance
	/// </summary>
	/// <returns>ID to access to created opaque instance</returns>
	uint32_t MeshSystem::createOpaqueInstance(const std::vector<std::array<std::wstring, 5>>& shaderFileNames)
	{
		std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
		opaqueInstance->setShaders(shaderFileNames);
		opaqueInstances.push_back(opaqueInstance);
		return (opaqueInstances.size() - 1);
	}

	MeshSystem::RenderMode MeshSystem::getRenderMode(const std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>& shaderGroup)
	{
		if (std::any_of(shaderGroup.begin(), shaderGroup.end(),
			[](const std::wstring& shaderName) { return shaderName.find(L"normalVisualizer") != std::wstring::npos; }))
		{
			return RenderMode::NORMAL_VISUALISER;
		}

		if (std::any_of(shaderGroup.begin(), shaderGroup.end(),
			[](const std::wstring& shaderName) { return shaderName.find(L"hologram") != std::wstring::npos; }))
		{
			return RenderMode::HOLOGRAM;
		}

		return RenderMode::DEFAULT;
	}

	void MeshSystem::setShaders(const std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>& shaderGroup)
	{
		//vertex shader
		ShaderManager::getInstance().getVertexShader(shaderGroup.at((int)OpaqueInstances::ShaderType::VERTEX_SHADER))->bind();

		//hull shader
		if (!shaderGroup.at((int)OpaqueInstances::ShaderType::HULL_SHADER).empty())
		{
			ShaderManager::getInstance().getHullShader(shaderGroup.at((int)OpaqueInstances::ShaderType::HULL_SHADER))->bind();
		}
		else
		{
			g_devcon->HSSetShader(nullptr, nullptr, 0);
		}

		//domain shader
		if (!shaderGroup.at((int)OpaqueInstances::ShaderType::DOMAIN_SHADER).empty())
		{
			ShaderManager::getInstance().getDomainShader(shaderGroup.at((int)OpaqueInstances::ShaderType::DOMAIN_SHADER))->bind();
		}
		else
		{
			g_devcon->DSSetShader(nullptr, nullptr, 0);
		}

		//geometry shader
		if (!shaderGroup.at((int)OpaqueInstances::ShaderType::GEOMETRY_SHADER).empty())
		{
			ShaderManager::getInstance().getGeometryShader(shaderGroup.at((int)OpaqueInstances::ShaderType::GEOMETRY_SHADER))->bind();
		}
		else
		{
			g_devcon->GSSetShader(nullptr, nullptr, 0);
		}

		//pixel shader
		ShaderManager::getInstance().getPixelShader(shaderGroup.at((int)OpaqueInstances::ShaderType::PIXEL_SHADER))->bind();
	}

	void MeshSystem::setPrimitiveTopology(const std::array<std::wstring, (int)OpaqueInstances::ShaderType::SHADER_TYPE_NUM>& shaderGroup)
	{
		//topology
		if (!shaderGroup.at((int)OpaqueInstances::ShaderType::HULL_SHADER).empty() && !shaderGroup.at((int)OpaqueInstances::ShaderType::DOMAIN_SHADER).empty())
		{
			g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		}
		else
		{
			g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}

}