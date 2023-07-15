#pragma once


#include "BaseInstances.h"


namespace engine::DX
{

	class OpaqueInstances : public BaseInstances
	{
	public:

		struct Material
		{			
			std::wstring material; // skin folder
		};

		struct Instance
		{
			TransformSystem::ID worldMatrixID;
			float4 color;
		};

	protected:

		struct PerMaterial
		{
			std::shared_ptr<Material> material;
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

		struct InstanceData
		{
			float4x4 worldMatrix;
			float4x4 invWorldMatrix;
			float4 color;
		};


		std::vector<PerModel> perModel{};
		VertexBuffer<InstanceData> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		ConstantBuffer<float4x4> meshData; // e.g. mesh to model transformation matrix
		ConstantBuffer<Material> materialData;


	public:
		OpaqueInstances();

		void updateInstanceBuffer() override;
		
		void render() override;

		void addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<Instance>& instances);



	};

} // namespace engine::DX


