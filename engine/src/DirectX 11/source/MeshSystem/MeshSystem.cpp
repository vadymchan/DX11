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
				if (!showNormal && shaderGroup.at(3).find(L"normalVisualizer") != std::wstring::npos)
				{
					continue;
				}

				//vertex shader
				ShaderManager::getInstance().getVertexShader(shaderGroup.at(0))->bind();

				//hull shader
				if (!shaderGroup.at(1).empty())
				{

					ShaderManager::getInstance().getHullShader(shaderGroup.at(1))->bind();
				}
				else
				{
					g_devcon->HSSetShader(nullptr, nullptr, 0);
				}

				//domain shader
				if (!shaderGroup.at(2).empty())
				{
					ShaderManager::getInstance().getDomainShader(shaderGroup.at(2))->bind();
				}
				else
				{
					g_devcon->DSSetShader(nullptr, nullptr, 0);
				}

				//geometry shader
				if (!shaderGroup.at(3).empty())
				{
					ShaderManager::getInstance().getGeometryShader(shaderGroup.at(3))->bind();
				}
				else
				{
					g_devcon->GSSetShader(nullptr, nullptr, 0);
				}

				//pixel shader
				ShaderManager::getInstance().getPixelShader(shaderGroup.at(4))->bind();


				if (shaderGroup.at(4).find(L"hologram") != std::wstring::npos)
				{
					float3 cameraPos = camera.position();
					cameraPosition.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {cameraPos.x, cameraPos.y, cameraPos.z, 1.0}});

					time.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {general::FPSTimer::getCurrentTick() - general::FPSTimer::initTick, 0, 0, 0}});
					cameraPosition.setPixelShaderBuffer();
					camera.setCameraBufferGeometryShader();
					time.setBuffer();
				}
				opaqueInstance->hasTexture(shaderGroup.at(4).find(L"color") != std::wstring::npos);

				//topology
				if (!shaderGroup.at(1).empty() && !shaderGroup.at(2).empty())
				{
					g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
				}
				else
				{
					g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				}

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

}