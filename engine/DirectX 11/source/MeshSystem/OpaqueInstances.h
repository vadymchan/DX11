#pragma once

#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <algorithm>
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
		ConstantBuffer<float4x4> meshData; // e.g. mesh to model transformation matrix
		ConstantBuffer<Material> materialData;
		std::shared_ptr<VertexShader> vertexShader;
		std::shared_ptr<PixelShader> pixelShader;
		std::wstring vertexShaderFileName;
		std::wstring pixelShaderFileName;
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

			instanceBuffer.initBuffer(INSTANCE_INPUT_SLOT_1, std::vector<UINT>{sizeof(Instance)}, std::vector<UINT>{0}, D3D11_USAGE_IMMUTABLE, instances);
			instanceBufferUpdated = true;
		}

		void needToUpdateInstanceBuffer()
		{
			instanceBufferUpdated = false;
		}

		void setVertexShader(const std::wstring& vertexShaderFileName)
		{
			vertexShader = ShaderManager::getInstance().getVertexShader(vertexShaderFileName);
			this->vertexShaderFileName = vertexShaderFileName;
		}

		void setPixelShader(const std::wstring& pixelShaderFileName)
		{
			pixelShader = ShaderManager::getInstance().getPixelShader(pixelShaderFileName);
			this->pixelShaderFileName = pixelShaderFileName;

		}

		const std::wstring& getVertexShaderFileName() const
		{
			return vertexShaderFileName;
		}

		const std::wstring& getPixelShaderFileName() const
		{
			return pixelShaderFileName;
		}

		void render(const DirectX::SimpleMath::Vector3& cameraPos = {})
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
					//meshData.update(mesh.meshToModel); // ... update shader local per-mesh uniform buffer

					for (const auto& perMaterial : model.perMesh.at(meshIndex).perMaterial)
					{
						if (perMaterial.instances.empty())
							continue;


						if (getVertexShaderFileName().find(L"hologram") == std::wstring::npos)
						{
							materialData.setBufferData(std::vector<Material>{ *perMaterial.material.get() });
							materialData.setPixelShaderBuffer();
						}

						if (vertexShaderFileName.find(L"hologram") != std::wstring::npos)
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




		std::vector<Instance*> addInstances(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<Material>& material, const std::vector<Instance>& instances)
		{
			std::vector<Instance*> returnInstances(instances.size());
			size_t initSize;
			//bool meshNumberDiscrepancy{ model->getMeshesCount() < obj.perMesh.size() };
			//assert(meshNumberDiscrepancy && "Number of meshes in mesh system are more than in the given model");
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
							std::transform(begin(mat.instances) + initSize, end(mat.instances), begin(returnInstances), [](Instance& i) {return &i; });
							return returnInstances;
						}
					}
					//didn't find such material in opaque instance. Create and add a new one
					PerMaterial newPerMaterial;
					initSize = newPerMaterial.instances.size() ;
					newPerMaterial.instances.reserve(instances.size());
					newPerMaterial.instances.insert(end(newPerMaterial.instances), begin(instances), end(instances));
					std::transform(begin(newPerMaterial.instances) + initSize, end(newPerMaterial.instances), begin(returnInstances), [](Instance& i) {return &i; });
					newPerMaterial.material = material;
					mesh.perMaterial.emplace_back(std::move(newPerMaterial));
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
			std::transform(begin(newPerMaterial.instances) + initSize, end(newPerMaterial.instances), begin(returnInstances), [](Instance& i) {return &i; });
			newPerMaterial.material = material;
			newPerMesh.perMaterial.emplace_back(std::move(newPerMaterial));
			perModel.emplace_back(std::move(newPerModel));
			return returnInstances;
		}



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


