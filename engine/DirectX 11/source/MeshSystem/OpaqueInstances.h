#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
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
			DirectX::XMFLOAT4 color;
		};

		struct Instance
		{
			DirectX::XMMATRIX toWorldMatrix;
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
			std::vector<PerMesh> perMesh{}; // for every mesh we have its own materials and instances
		};

		std::vector<PerModel> perModel{};
		VertexBuffer<Instance> instanceBuffer; // mesh instances in GPU (for rendering one mesh several instances)
		//ConstantBuffer<> meshData; // e.g. mesh to model transformation matrix
		//ConstantBuffer<Material> materialData; // e.g. time that needed to change material effect
		std::shared_ptr<VertexShader> vertexShader;
		std::shared_ptr<PixelShader> pixelShader;
		bool instanceBufferUpdated{};
		
	public:
		void updateInstanceBuffer()
		{
			/*uint32_t totalInstances = 0;

			std::vector<Instance> instances;
			for (const auto& model : perModel)
			{
				for (const auto& mesh : model.perMesh)
				{
					for (const auto& material : mesh.perMaterial)
					{
						++totalInstances;
					}
				}
			}

			instances.reserve(totalInstances);
			instanceBuffer.setSizeOfBuffer(totalInstances);
			for (const auto& model : perModel)
			{
				for (const auto& mesh : model.perMesh)
				{
					for (const auto& material : mesh.perMaterial)
					{
						instances.insert(end(instances), begin(material.instances), end(material.instances));
					}
				}
			}*/
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
							instances.push_back(instance);
						}
					}
				}
			}

			/*instanceBuffer.setBufferData(instances);
			instanceBuffer.setSizeOfBuffer(instances.size());*/
			//instanceBuffer.initBuffer(INSTANCE_INPUT_SLOT_1, sizeof(Instance), 0, instances.size() * sizeof(Instance), D3D11_USAGE_IMMUTABLE, instances.data());
			instanceBuffer.initBuffer(INSTANCE_INPUT_SLOT_1, sizeof(Instance), 0, D3D11_USAGE_IMMUTABLE, instances);
			instanceBufferUpdated = true;
		}

		void setVertexShader(const std::wstring& path)
		{
			vertexShader = ShaderManager::getInstance().getVertexShader(path);
		}
		
		void setPixelShader(const std::wstring& path)
		{
			pixelShader = ShaderManager::getInstance().getPixelShader(path);
		}

		void updateMaterialData()
		{

		}

		void render()
		{

			updateInstanceBuffer();
			if (instanceBuffer.empty())
			{
				return;
			}


			vertexShader->bind(); 
			pixelShader->bind();
			instanceBuffer.setBuffer();	

			for (const auto& model : perModel)
			{
				if (model.perMesh.empty())
					continue;


				model.model->bindBuffers(MODEL_DATA_INPUT_SLOT_0);

				for (size_t meshIndex = 0; meshIndex < model.perMesh.size(); ++meshIndex)
				{
					const Mesh& mesh = model.model->getMesh(meshIndex);
					const Model::MeshRange& meshRange = model.model->getMeshRange(meshIndex);
					int renderedModelIndexes = 0;
					
					//meshData.update(mesh.meshToModel); // ... update shader local per-mesh uniform buffer

					for (const auto& perMaterial : model.perMesh.at(meshIndex).perMaterial)
					{
						if (perMaterial.instances.empty())
							continue;

						/*const std::shared_ptr<Material>& material = perMaterial.material;
						materialData.update();*/


						int renderedInstance = 0;


						//g_devcon->DrawIndexedInstanced(meshRange.indexNum, perMaterial.instances.size(), meshRange.indexOffset, meshRange.vertexOffset, renderedInstance);
						
						g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
						g_devcon->DrawIndexedInstanced(meshRange.indexNum, perMaterial.instances.size(), renderedInstance, meshRange.vertexOffset, renderedInstance);


						renderedModelIndexes += meshRange.indexNum;
						renderedInstance += perMaterial.instances.size();
					}
				}
			}

		}



		/// <summary>
		/// adds new instances to the current opaque instance.
		/// Attention: add instances to certain material in one batch. Otherwise the same material will be created but with new instances
		/// </summary>
		void addInstances(const std::shared_ptr<Model>& model, size_t meshIndex,  const std::shared_ptr<Material>& material, const std::vector<Instance>& instances)
		{
		
			//bool meshNumberDiscrepancy{ model->getMeshesCount() < obj.perMesh.size() };
			//assert(meshNumberDiscrepancy && "Number of meshes in mesh system are more than in the given model");

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
					mesh.perMaterial.emplace_back(std::move(newPerMaterial));
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
			newPerMesh.perMaterial.emplace_back(std::move(newPerMaterial));
			perModel.emplace_back(std::move(newPerModel));
			
		}

		OpaqueInstances()
		{
		}

	};
}


