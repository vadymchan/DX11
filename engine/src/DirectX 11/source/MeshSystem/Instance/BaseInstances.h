#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <algorithm>
#include "../Model.h"
#include "../../Buffer/ConstantBuffer.h"
#include "../../Controller/ShaderManager.h"
#include "../../Controller/BufferManager.h"
#include "../../Controller/TextureManager.h"
#include "../../Math/Transform/TransformSystem.h"



namespace engine::DX
{

	class BaseInstances
	{
	public:

		enum class ShaderType
		{
			VERTEX_SHADER,
			HULL_SHADER,
			DOMAIN_SHADER,
			GEOMETRY_SHADER,
			PIXEL_SHADER,
			SHADER_TYPE_NUM

		};

		enum class RenderType
		{
			DEFAULT,
			NORMAL_VISUALIZER,
			HOLOGRAM,
			BLINN_PHONG,
			POINT_SPHERE,
			PBR,
			IBL,
			SHADOW_GENERATION,
			IBL_SHADOW,
			DISSOLUTION_SHADOW_GENERATION,
			IBL_DISSOLUTION
		};


		struct ShaderGroup
		{
			RenderType type;

			std::vector<std::weak_ptr<Texture2D>> shaderResources;

			std::weak_ptr<VertexShader> vertexShader;
			std::weak_ptr<HullShader> hullShader;
			std::weak_ptr<DomainShader> domainShader;
			std::weak_ptr<GeometryShader> geometryShader;
			std::weak_ptr<PixelShader> pixelShader;
		};


	protected:


		std::vector<ShaderGroup> shaders;
		bool instanceBufferUpdated{};

	public:
		virtual void updateInstanceBuffer() = 0;

		void needToUpdateInstanceBuffer();

		void setShaders(const std::vector<std::array<std::wstring, (int)ShaderType::SHADER_TYPE_NUM>>& shaderBatches);

		const std::vector<ShaderGroup>& getShaders() const { return shaders; }

		virtual void render() = 0;


	};

} // namespace engine::DX


