#pragma once


#include "BaseInstances.h"



namespace engine::DX
{

	class DissolutionInstances : public BaseInstances
	{
	public:

		const static float SPAWN_DISTANCE;

		struct Material
		{			
			std::wstring material; // skin folder
		};

		struct Instance
		{
			TransformSystem::ID worldMatrixID;
			float time;
		};

		struct ModelInstanceID
		{
			std::shared_ptr<Model> model;
			std::vector<size_t> meshIndices;
			std::shared_ptr<Material> material;
			TransformSystem::ID transformID;
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

		struct InstanceData
		{
			float4x4 worldMatrix;
			float4x4 invWorldMatrix;
			float time;
		};


		std::vector<PerModel> perModel{};
		VertexBuffer<InstanceData> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		ConstantBuffer<float4x4> meshData; // e.g. mesh to model transformation matrix
		ConstantBuffer<Material> materialData;


	public:

		DissolutionInstances();

		void updateInstanceBuffer() override;
		 
		void render() override;

		void addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<Instance>>& instances);

		void removeModelInstance(const ModelInstanceID& descriptor);

		std::shared_ptr<Instance> getDissolutionInstance(const ModelInstanceID& descriptor);

	};

} // namespace engine::DX


