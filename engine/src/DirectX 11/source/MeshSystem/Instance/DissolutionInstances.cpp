#include "DissolutionInstances.h"

namespace engine::DX
{

	const float DissolutionInstances::SPAWN_DISTANCE = 5.0f;

	DissolutionInstances::DissolutionInstances()
	{
		instanceBuffer.initBuffer(INSTANCE_INPUT_SLOT_1, std::vector<UINT>{sizeof(InstanceData)}, std::vector<UINT>{0}, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		materialData.initBuffer(MATERIAL_NORMAL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		meshData.initBuffer(MESH_TO_MODEL_SHADER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	}

	void DissolutionInstances::updateInstanceBuffer()
	{

		std::vector<InstanceData> instances;
		for (const auto& model : perModel)
		{
			for (const auto& mesh : model.perMesh)
			{
				for (const auto& material : mesh.perMaterial)
				{
					for (const auto& instance : material.instances)
					{
						float4x4 worldMatrix = TransformSystem::getInstance().getTransform(instance->worldMatrixID);
						instances.emplace_back(InstanceData{ worldMatrix, worldMatrix.Invert(),  instance->time });
					}
				}
			}
		}

		instanceBuffer.updateBuffer(instances);
		instanceBufferUpdated = true;
	}

	void DissolutionInstances::render()
	{

		updateInstanceBuffer();
		if (instanceBuffer.empty())
		{
			return;
		}


		instanceBuffer.setBuffer();
		int renderedInstance = 0;
		for (const auto& model : perModel)
		{
			if (model.perMesh.empty())
				continue;


			model.model->bindBuffers(MODEL_DATA_INPUT_SLOT_0);
			int renderedModelIndexes = 0;
			int renderedModelVertexes = 0;
			for (size_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
			{
				const Mesh& mesh = model.model->getMesh(meshIndex);
				const Model::MeshRange& meshRange = model.model->getMeshRange(meshIndex);


				meshData.setBufferData(mesh.getMeshToModelMat());
				meshData.setVertexShaderBuffer();
				meshData.setGeometryShaderBuffer();

				for (const auto& perMaterial : model.perMesh.at(meshIndex).perMaterial)
				{
					if (perMaterial.instances.empty())
						continue;

					const auto& textureFolder = perMaterial.material.get()->material;
					const auto& textures = mesh.getMaterialNames(textureFolder);


					for (const auto& texture : textures)
					{
						texture.lock()->bind();
					}

					g_devcon->DrawIndexedInstanced(meshRange.indexNum, perMaterial.instances.size(), renderedModelIndexes, renderedModelVertexes, renderedInstance);


					renderedInstance += perMaterial.instances.size();
				}
				renderedModelIndexes += meshRange.indexNum;
				renderedModelVertexes += meshRange.vertexNum;
			}


			UnbindVertexBuffer(MODEL_DATA_INPUT_SLOT_0);
		}
	}

	void DissolutionInstances::addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<Instance>>& instances)
	{
		assert(meshIndex <= model->getMeshesCount() && "Number of meshes in mesh system are more than in the given model");
		instanceBufferUpdated = false;

		for (auto& obj : perModel)
		{
			if (obj.model.get() == model.get()) // model is already in opaque instance
			{
				PerMesh& mesh = obj.perMesh[meshIndex];
				for (auto& mat : mesh.perMaterial)
				{
					if (mat.material.get() == material.get()) // material is already in opaque instance
					{
						mat.instances.reserve(mat.instances.size() + instances.size());
						mat.instances.insert(end(mat.instances), begin(instances), end(instances));
						return;
					}
				}
				// didn't find such material in opaque instance. Create and add a new one
				PerMaterial newPerMaterial;
				newPerMaterial.instances.reserve(instances.size());
				newPerMaterial.instances.insert(end(newPerMaterial.instances), begin(instances), end(instances));
				newPerMaterial.material = material;
				mesh.perMaterial.emplace_back(newPerMaterial);
				return;
			}


		}
		// no such model in current opaque instance. Create a new model and add to a particular mesh material and instances
		PerModel newPerModel;
		newPerModel.model = model;
		newPerModel.perMesh.resize(model->getMeshesCount());
		PerMesh& newPerMesh = newPerModel.perMesh.at(meshIndex);
		PerMaterial newPerMaterial;
		newPerMaterial.instances.reserve(instances.size());
		newPerMaterial.instances.insert(end(newPerMaterial.instances), begin(instances), end(instances));
		newPerMaterial.material = material;
		newPerMesh.perMaterial.emplace_back(newPerMaterial);
		perModel.emplace_back(newPerModel);
		return;
	}

	void DissolutionInstances::removeModelInstance(const ModelInstanceID& descriptor)
	{
		instanceBufferUpdated = false;

		for (auto& modelInstanceGroup : perModel)
		{
			if (modelInstanceGroup.model.get() == descriptor.model.get()) // model found
			{
				bool instanceRemoved = false;
				for (size_t meshIndex = 0; meshIndex < descriptor.meshIndices.size(); ++meshIndex)
				{
					PerMesh& meshInstanceGroup = modelInstanceGroup.perMesh[descriptor.meshIndices[meshIndex]];
					for (auto& materialInstanceGroup : meshInstanceGroup.perMaterial)
					{
						if (materialInstanceGroup.material.get() == descriptor.material.get()) // material found
						{
							for (size_t instanceIndex = 0; instanceIndex < materialInstanceGroup.instances.size(); ++instanceIndex)
							{
								if (materialInstanceGroup.instances[instanceIndex]->worldMatrixID == descriptor.transformID) // instance found
								{
									materialInstanceGroup.instances.erase(materialInstanceGroup.instances.begin() + instanceIndex);
									instanceRemoved = true;
									break;
								}
							}
						}
					}
				}
				if (instanceRemoved) // all meshes for specified model instance are removed 
				{
					return;
				}
			}
		}

		std::cerr << "Instance not found. Nothing to remove\n";
	}

	std::shared_ptr<DissolutionInstances::Instance> DissolutionInstances::getDissolutionInstance(const ModelInstanceID& descriptor)
	{
		for (auto& modelInstanceGroup : perModel)
		{
			if (modelInstanceGroup.model.get() == descriptor.model.get()) // model found
			{
				for (size_t meshIndex = 0; meshIndex < descriptor.meshIndices.size(); ++meshIndex)
				{
					PerMesh& meshInstanceGroup = modelInstanceGroup.perMesh[descriptor.meshIndices[meshIndex]];
					for (auto& materialInstanceGroup : meshInstanceGroup.perMaterial)
					{
						if (materialInstanceGroup.material.get() == descriptor.material.get()) // material found
						{
							for (size_t instanceIndex = 0; instanceIndex < materialInstanceGroup.instances.size(); ++instanceIndex)
							{
								if (materialInstanceGroup.instances[instanceIndex]->worldMatrixID == descriptor.transformID) // instance found
								{
									return materialInstanceGroup.instances[instanceIndex];
								}
							}
						}
					}
				}
			}
		}

		return nullptr;
	}

}