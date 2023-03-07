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
			std::vector<std::shared_ptr<Instance>> instances{}; //to avoid dangling pointers in triangleOctree while reallocating
		};

		struct PerMesh
		{
			std::vector<PerMaterial> perMaterial{};
		};

		struct PerModel
		{
			std::shared_ptr<Model> model;
			std::vector<PerMesh> perMesh{}; // for every mesh we have its own materials and instances
		};

		std::vector<PerModel> perModel{};
		VertexBuffer<Instance> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		ConstantBuffer<float4x4> meshData; // e.g. mesh to model transformation matrix
		ConstantBuffer<Material> materialData;
		std::shared_ptr<VertexShader> vertexShader;
		std::shared_ptr<GeometryShader> geometryShader;
		std::shared_ptr<PixelShader> pixelShader;
		std::wstring vertexShaderFileName;
		std::wstring pixelShaderFileName;
		bool instanceBufferUpdated{};

	public:
		void updateInstanceBuffer();

		void needToUpdateInstanceBuffer();

		void setVertexShader(const std::wstring& vertexShaderFileName);

		void setPixelShader(const std::wstring& pixelShaderFileName);

		const std::wstring& getVertexShaderFileName() const;

		const std::wstring& getPixelShaderFileName() const
		{
			return pixelShaderFileName;
		}

		void render(const DirectX::SimpleMath::Vector3& cameraPos = {});




		std::vector<std::weak_ptr<Instance>> addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<Instance>>& instances);



		OpaqueInstances()
		{
			materialData.initBuffer(MATERIAL_NORMAL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
			cameraPosition.initBuffer(PER_VIEW_PIXEL_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
			time.initBuffer(PER_DRAW_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
			meshData.initBuffer(MESH_TO_MODEL_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
		}

		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;
	};
}


