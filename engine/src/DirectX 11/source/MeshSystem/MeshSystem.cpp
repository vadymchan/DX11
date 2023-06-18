#include "MeshSystem.h"


namespace engine::DX
{

	void MeshSystem::render(Camera& camera)
	{
		for (const auto& opaqueInstance : opaqueInstances)
		{
			//bind shaders (draw several times with different shader)
			for (auto& shaderGroup : opaqueInstance->getShaders())
			{
				RenderMode renderModeType = getRenderMode(shaderGroup);

				setShaders(shaderGroup);

				switch (renderModeType)
				{
				case RenderMode::DEFAULT:
					SetRenderMode<RenderMode::DEFAULT>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::NORMAL_VISUALISER:
					if (!m_showNormal)
						continue;
					SetRenderMode<RenderMode::NORMAL_VISUALISER>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::HOLOGRAM:
					SetRenderMode<RenderMode::HOLOGRAM>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::BLINN_PHONG:
					SetRenderMode<RenderMode::BLINN_PHONG>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::POINT_SPHERE:
					SetRenderMode<RenderMode::POINT_SPHERE>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::PBR:
					SetRenderMode<RenderMode::PBR>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::IBL_SHADOW:
					SetRenderMode<RenderMode::IBL_SHADOW>(opaqueInstance.get(), &camera);
					break;
				case RenderMode::IBL:
					SetRenderMode<RenderMode::IBL>(opaqueInstance.get(), &camera);
					break;
				}

				
				
				setPrimitiveTopology(shaderGroup);


				opaqueInstance->render();
			}
		}

		//render UI component
		ImGuiManager::getInstance().RenderCheckbox("useIBL", m_perFrameIBL.useIBL);
		if (m_perFrameIBL.useIBL)
		{
			ImGuiManager::getInstance().RenderCheckbox("useDiffuseReflection", m_perFrameIBL.useDiffuseReflection);
			ImGuiManager::getInstance().RenderCheckbox("useSpecularReflection", m_perFrameIBL.useSpecularReflection);
		}
		
		ImGuiManager::getInstance().RenderCheckbox("useRoughnessOverwriting", m_perFrameIBL.useRoughnessOverwriting);
		if (m_perFrameIBL.useRoughnessOverwriting)
		{
			ImGuiManager::getInstance().RenderSlider("overwrittenRoughnessValue", m_perFrameIBL.overwrittenRoughnessValue, 0.0f, 1.0f);
		}
	}

	void MeshSystem::renderDepth2D()
	{
		const static std::wstring shadowMapVertexShaderFileName{ L"Shadow/2D/shadow2DVertexShader.hlsl" };
		const static std::wstring shadowMapPixelShaderFileName{ L"Shadow/2D/shadow2DPixelShader.hlsl" };

		static OpaqueInstances::ShaderGroup shaderGroup
		{
			OpaqueInstances::RenderType::SHADOW_GENERATION,
			{},
			ShaderManager::getInstance().getVertexShader(shadowMapVertexShaderFileName),
			{},
			{},
			{},
			ShaderManager::getInstance().getPixelShader(shadowMapPixelShaderFileName),
		};

		setShaders(shaderGroup);
		for (const auto& opaqueInstance : opaqueInstances)
		{
			SetRenderMode<RenderMode::SHADOW_GENERATION>(opaqueInstance.get());
			setPrimitiveTopology(shaderGroup);
			opaqueInstance->render();
		}
	}

	void MeshSystem::renderDepthCubemap()
	{
		const static std::wstring shadowCubeMapVertexShaderFileName{ L"Shadow/Omnidirectional/shadow3DVertexShader.hlsl" };
		const static std::wstring shadowCubeMapGeometryShaderFileName{ L"Shadow/Omnidirectional/shadow3DGeometryShader.hlsl" };
		const static std::wstring shadowCubeMapPixelShaderFileName{ L"Shadow/Omnidirectional/shadow3DPixelShader.hlsl" };


		static OpaqueInstances::ShaderGroup shaderGroup
		{
			OpaqueInstances::RenderType::SHADOW_GENERATION,
			{},
			ShaderManager::getInstance().getVertexShader(shadowCubeMapVertexShaderFileName),
			{},
			{},
			ShaderManager::getInstance().getGeometryShader(shadowCubeMapGeometryShaderFileName),
			ShaderManager::getInstance().getPixelShader(shadowCubeMapPixelShaderFileName),
		};

		setShaders(shaderGroup);
		for (const auto& opaqueInstance : opaqueInstances)
		{
			SetRenderMode<RenderMode::SHADOW_GENERATION>(opaqueInstance.get());
			setPrimitiveTopology(shaderGroup);
			opaqueInstance->render();
		}

	}




	/// <param name="instance">instance, which is contained in opaqueInstance</param>
	bool MeshSystem::findIntersection(const ray& r, Instance& instance, Intersection& intersection)
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
	void MeshSystem::updateOpaqueInstanceBuffer(const Instance instance)
	{
		for (auto& modelIntersection : modelIntersections)
		{
			if (modelIntersection.instance.worldMatrixID == instance.worldMatrixID)
			{
				modelIntersection.opaqueInstance.lock()->needToUpdateInstanceBuffer();
			}
		}
	}

	void MeshSystem::addInstances(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<Instance>& transformID)
	{
		bool debugCubeModel{};

		std::vector<Instance> instances{};
		instances.resize(transformID.size());
		for (size_t i = 0; i < transformID.size(); i++)
		{
			instances[i] = Instance{ transformID[i] };
		}

		for (size_t meshIndex = 0; meshIndex < meshIndices.size(); ++meshIndex)
		{

			opaqueInstances.at(opaqueInstanceID)->addInstances(model, meshIndices[meshIndex], material, instances);

			if (model->getMesh(meshIndex).GetName() == ModelManager::debugCubeTag)
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
							ModelIntersection{ triangleOctrees[triangleOctreeIndex], instanceID, opaqueInstances[opaqueInstanceID] });
						break;
					}
				}

				if (triangleOctreeIndex == triangleOctrees.size())
				{
					triangleOctrees.emplace_back(std::make_shared<ModelTriangleOctree>(model, meshIndices));
					modelIntersections.emplace_back(
						ModelIntersection{ triangleOctrees[triangleOctrees.size() - 1], instanceID, opaqueInstances[opaqueInstanceID] });
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
		opaqueInstances.emplace_back(opaqueInstance);
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
		case OpaqueInstances::RenderType::BLINN_PHONG:
			return RenderMode::BLINN_PHONG;
		case OpaqueInstances::RenderType::POINT_SPHERE:
			return RenderMode::POINT_SPHERE;
		case OpaqueInstances::RenderType::PBR:
			return RenderMode::PBR;
		case OpaqueInstances::RenderType::IBL_SHADOW:
			return RenderMode::IBL_SHADOW;
		case OpaqueInstances::RenderType::IBL:
			return RenderMode::IBL;
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


		//set additional resources for the shader
		for (auto& shaderResource : shaderGroup.shaderResources)
		{
			if (!shaderResource.expired())
			{
				shaderResource.lock()->bind();
			}
		}

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