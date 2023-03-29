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

		struct Material
		{
			DirectX::SimpleMath::Vector4 color;
		};

		struct Instance
		{
			float4x4 toWorldMatrix;
		};
	protected:

		struct PerMaterial
		{
			std::shared_ptr<Material> material;
			std::vector<std::shared_ptr<Instance>> instances{}; 
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
		VertexBuffer<Instance> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		ConstantBuffer<float4x4> meshData; // e.g. mesh to model transformation matrix
		ConstantBuffer<Material> materialData;
		
		std::vector<std::array<std::wstring, (int)ShaderType::SHADER_TYPE_NUM>> shaders;
		bool instanceBufferUpdated{};
		bool m_hasTexture{};
	public:
		void updateInstanceBuffer();

		void needToUpdateInstanceBuffer();

		void setShaders(const std::vector<std::array<std::wstring, (int)ShaderType::SHADER_TYPE_NUM>>& shaderBatches);

		void hasTexture(bool value);

		const std::vector<std::array<std::wstring, (int)ShaderType::SHADER_TYPE_NUM>>& getShaders() const
		{
			return shaders;
		}

		void render();

		void addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<Instance>>& instances);

		OpaqueInstances()
		{
			materialData.initBuffer(MATERIAL_NORMAL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			meshData.initBuffer(MESH_TO_MODEL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		}

		
	};
}


