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
	
	void OpaqueInstances::setVertexShader(const std::wstring& vertexShaderFileName)
	{
		vertexShader = ShaderManager::getInstance().getVertexShader(vertexShaderFileName);
		this->vertexShaderFileName = vertexShaderFileName;
	}
	
	void OpaqueInstances::setPixelShader(const std::wstring& pixelShaderFileName)
	{
		pixelShader = ShaderManager::getInstance().getPixelShader(pixelShaderFileName);
		this->pixelShaderFileName = pixelShaderFileName;

	}
	
	const std::wstring& OpaqueInstances::getVertexShaderFileName() const
	{
		return vertexShaderFileName;
	}
	
	void OpaqueInstances::render(const DirectX::SimpleMath::Vector3& cameraPos)
	{

		updateInstanceBuffer();
		if (instanceBuffer.empty())
		{
			return;
		}

		vertexShader->bind();
		pixelShader->bind();
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

				for (const auto& perMaterial : model.perMesh.at(meshIndex).perMaterial)
				{
					if (perMaterial.instances.empty())
						continue;


					if (getVertexShaderFileName().find(L"hologram") == std::wstring::npos)
					{
						materialData.setBufferData(std::vector<Material>{ *perMaterial.material.get() });
						materialData.setPixelShaderBuffer();
					}

					if (vertexShaderFileName.find(L"normal") == std::wstring::npos)
					{
						cameraPosition.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {cameraPos.x, cameraPos.y, cameraPos.z, 1.0}});
						time.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {static_cast<float>(GetTickCount64() / 1000.f), 0, 0, 0}});
						cameraPosition.setPixelShaderBuffer();
						time.setBuffer();
					}

					g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					g_devcon->DrawIndexedInstanced(meshRange.indexNum, perMaterial.instances.size(), renderedModelIndexes, renderedModelVertexes, renderedInstance);


					renderedInstance += perMaterial.instances.size();
				}
				renderedModelIndexes += meshRange.indexNum;
				renderedModelVertexes += meshRange.vertexNum;
			}
		}

	}
	
	std::vector<std::weak_ptr<OpaqueInstances::Instance>> OpaqueInstances::addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<std::shared_ptr<Instance>>& instances)
	{
		std::vector<std::weak_ptr<Instance>> returnInstances;
		size_t initSize;
		assert( meshIndex <= model->getMeshesCount() && "Number of meshes in mesh system are more than in the given model");
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
						initSize = mat.instances.size();
						mat.instances.reserve(initSize + instances.size());
						mat.instances.insert(end(mat.instances), begin(instances), end(instances));
						returnInstances.insert(end(returnInstances), begin(mat.instances), end(mat.instances));
						return returnInstances;
					}
				}
				//didn't find such material in opaque instance. Create and add a new one
				PerMaterial newPerMaterial;
				initSize = newPerMaterial.instances.size();
				newPerMaterial.instances.reserve(instances.size());
				newPerMaterial.instances.insert(end(newPerMaterial.instances), begin(instances), end(instances));
				returnInstances.insert(end(returnInstances), begin(newPerMaterial.instances), end(newPerMaterial.instances));
				newPerMaterial.material = material;
				mesh.perMaterial.emplace_back(newPerMaterial);
				return returnInstances;
			}


		}
		// no such model in current opaque instance. Create a new model and add to a particular mesh material and instances
		PerModel newPerModel;
		newPerModel.model = model;
		newPerModel.perMesh.resize(model->getMeshesCount());
		PerMesh& newPerMesh = newPerModel.perMesh.at(meshIndex);
		PerMaterial newPerMaterial;
		initSize = newPerMaterial.instances.size();
		newPerMaterial.instances.reserve(instances.size());
		newPerMaterial.instances.insert(end(newPerMaterial.instances), begin(instances), end(instances));
		returnInstances.insert(end(returnInstances), begin(newPerMaterial.instances), end(newPerMaterial.instances));
		newPerMaterial.material = material;
		newPerMesh.perMaterial.emplace_back(newPerMaterial);
		perModel.emplace_back(newPerModel);
		return returnInstances;
	}
}