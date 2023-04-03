#include "OpaqueInstances.h"

namespace engine::DX
{
	void OpaqueInstances::updateInstanceBuffer()
	{
		if (instanceBufferUpdated)
			return;

		std::vector<Instance> instances;
		for (const auto& model : perModel)
		{
			for (const auto& mesh : model.perMesh)
			{
				for (const auto& material : mesh.perMaterial)
				{
					for (const auto& instance : material.instances)
					{
						instances.push_back(*instance.get());
					}
				}
			}
		}

		instanceBuffer.initBuffer(INSTANCE_INPUT_SLOT_1, std::vector<UINT>{sizeof(Instance)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE, instances);
		instanceBufferUpdated = true;
	}

	void OpaqueInstances::needToUpdateInstanceBuffer()
	{
		instanceBufferUpdated = false;
	}

	void OpaqueInstances::setShaders(const std::vector<std::array<std::wstring, 5>>& shaderBatches)
	{
		shaders = shaderBatches;
	}


	void OpaqueInstances::hasTexture(bool value)
	{
		m_hasTexture = value;
	}

	void OpaqueInstances::render()
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


					if (m_hasTexture)
					{ 
						const auto& textureFolder = perMaterial.material.get()->material;

						if (std::holds_alternative<float3>(textureFolder))
						{
							materialData.setBufferData(std::vector<Material>{ *perMaterial.material.get() });
							materialData.setPixelShaderBuffer();
						}
						else if (std::holds_alternative<std::wstring>(textureFolder))
						{
							const auto& textures = mesh.getMaterialNames(std::get<std::wstring>(textureFolder));
							for (const auto& texture : textures)
							{
								texture.lock()->bind();
							}




						}

						
						

						
					}




					g_devcon->DrawIndexedInstanced(meshRange.indexNum, perMaterial.instances.size(), renderedModelIndexes, renderedModelVertexes, renderedInstance);


					renderedInstance += perMaterial.instances.size();
				}
				renderedModelIndexes += meshRange.indexNum;
				renderedModelVertexes += meshRange.vertexNum;
			}
		}

	}

	void OpaqueInstances::addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<Instance>>& instances)
	{
		assert(meshIndex <= model->getMeshesCount() && "Number of meshes in mesh system are more than in the given model");
		instanceBufferUpdated = false;

		for (auto& obj : perModel)
		{
			if (obj.model.get() == model.get()) // model is already in opaque instance
			{
				PerMesh& mesh = obj.perMesh.at(meshIndex);
				for (auto& mat : mesh.perMaterial)
				{
					if (mat.material.get() == material.get()) // material is already in opaque instance
					{
						mat.instances.reserve(mat.instances.size() + instances.size());
						mat.instances.insert(end(mat.instances), begin(instances), end(instances));
						return;
					}
				}
				//didn't find such material in opaque instance. Create and add a new one
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
}