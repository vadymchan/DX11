#include "BaseInstances.h"

namespace engine::DX
{
	

	void BaseInstances::needToUpdateInstanceBuffer()
	{
		instanceBufferUpdated = false;
	}



	void BaseInstances::setShaders(const std::vector<std::array<std::wstring, (int)BaseInstances::ShaderType::SHADER_TYPE_NUM>>& shaderBatches)
	{
		shaders.clear();
		shaders.resize(shaderBatches.size());

		for (size_t i = 0; i < shaderBatches.size(); ++i)
		{
			ShaderGroup shaderGroup;

			//assign render type
			if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"normalVisualizer") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::NORMAL_VISUALIZER;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"hologram") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::HOLOGRAM;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"Blinn-Phong") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::BLINN_PHONG;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"pointLight") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::POINT_SPHERE;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"PBR") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::PBR;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"IBL_Shadow") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::IBL_SHADOW;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"IBL") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::IBL;
			}
			else if (std::any_of(shaderBatches[i].begin(), shaderBatches[i].end(),
				[](const std::wstring& shaderName) { return shaderName.find(L"IBL_Dissolution") != std::wstring::npos; }))
			{
				shaderGroup.type = RenderType::IBL_DISSOLUTION;
			}
			else
			{
				shaderGroup.type = RenderType::DEFAULT;
			}

			//add shaders
			//vertex shader
			shaderGroup.vertexShader = !shaderBatches[i][(int)BaseInstances::ShaderType::VERTEX_SHADER].empty()
				? ShaderManager::getInstance().getVertexShader(shaderBatches[i][(int)BaseInstances::ShaderType::VERTEX_SHADER])
				: std::weak_ptr<VertexShader>();
			//hull shader
			shaderGroup.hullShader = !shaderBatches[i][(int)BaseInstances::ShaderType::HULL_SHADER].empty()
				? ShaderManager::getInstance().getHullShader(shaderBatches[i][(int)BaseInstances::ShaderType::HULL_SHADER])
				: std::weak_ptr<HullShader>();
			//domain shader
			shaderGroup.domainShader = !shaderBatches[i][(int)BaseInstances::ShaderType::DOMAIN_SHADER].empty()
				? ShaderManager::getInstance().getDomainShader(shaderBatches[i][(int)BaseInstances::ShaderType::DOMAIN_SHADER])
				: std::weak_ptr<DomainShader>();
			//geometry shader
			shaderGroup.geometryShader = !shaderBatches[i][(int)BaseInstances::ShaderType::GEOMETRY_SHADER].empty()
				? ShaderManager::getInstance().getGeometryShader(shaderBatches[i][(int)BaseInstances::ShaderType::GEOMETRY_SHADER])
				: std::weak_ptr<GeometryShader>();
			//pixel shader
			shaderGroup.pixelShader = !shaderBatches[i][(int)BaseInstances::ShaderType::PIXEL_SHADER].empty()
				? ShaderManager::getInstance().getPixelShader(shaderBatches[i][(int)BaseInstances::ShaderType::PIXEL_SHADER])
				: std::weak_ptr<PixelShader>();

			shaders[i] = shaderGroup;
		}

	}


}