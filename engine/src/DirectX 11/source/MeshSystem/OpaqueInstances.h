#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <deque>
#include "Model.h"
#include "../Buffer/ConstantBuffer.h"
#include "../Controller/ShaderManager.h"
#include "../Controller/BufferManager.h"
#include "../Controller/TextureManager.h"
#include "../TransformSystem.h"
#include <variant>


namespace engine::DX
{
	class OpaqueInstances
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
		};


		struct ShaderGroup
		{
			RenderType type;

			std::weak_ptr<VertexShader> vertexShader;
			std::weak_ptr<HullShader> hullShader;
			std::weak_ptr<DomainShader> domainShader;
			std::weak_ptr<GeometryShader> geometryShader;
			std::weak_ptr<PixelShader> pixelShader;
		};

		struct Material
		{
			
			std::variant<float3, std::wstring> material; // float3 - color, std::wsting  - skin folder
			
		};

		struct Instance
		{
			TransformSystem::ID worldMatrixID;
			//float4x4 toWorldMatrix;
		};
	protected:

		struct PerMaterial
		{
			std::shared_ptr<Material> material;
			//std::vector<std::shared_ptr<Instance>> instances{};
			std::vector<Instance> instances{};
		};

		struct PerMesh
		{
			std::vector<PerMaterial> perMaterial{};
		};

		struct PerModel
		{
			std::shared_ptr<Model> model;
			std::vector<PerMesh> perMesh{};
		};

		


		std::vector<PerModel> perModel{};
		//VertexBuffer<Instance> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		VertexBuffer<float4x4> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		ConstantBuffer<float4x4> meshData; // e.g. mesh to model transformation matrix
		ConstantBuffer<Material> materialData;

		std::vector<ShaderGroup> shaders;
		bool instanceBufferUpdated{};
		bool m_hasTexture{};
	public:
		void updateInstanceBuffer();

		void needToUpdateInstanceBuffer();

		void setShaders(const std::vector<std::array<std::wstring, 5>>& shaderBatches);

		void SetTexture(bool useTexture);

		const std::vector<ShaderGroup>& getShaders() const
		{
			return shaders;
		}


		void render();

		void addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<Instance>& instances);

		OpaqueInstances()
		{
			materialData.initBuffer(MATERIAL_NORMAL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			meshData.initBuffer(MESH_TO_MODEL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		}


	};
}


