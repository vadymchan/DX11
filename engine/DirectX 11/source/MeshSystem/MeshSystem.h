#pragma once
#include "OpaqueInstances.h"
#include "../Math/Colision/TriangleOctreeDX.h"


namespace engine::DX
{
	using Intersection = TriangleOctree::Intersection;
	class MeshSystem
	{
	public:

		MeshSystem(const MeshSystem&) = delete;
		MeshSystem& operator=(const MeshSystem&) = delete;
		static MeshSystem& getInstance()
		{
			static MeshSystem instance;
			return instance;
		}

		void render(const DirectX::SimpleMath::Vector3& cameraPos = {})
		{
			for (const auto& opaqueInstance : opaqueInstances)
			{
				//if (opaqueInstance->getVertexShaderFileName().find(L"hologram") != std::wstring::npos)
				//{
				//	cameraPosition.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {cameraPos.x, cameraPos.y, cameraPos.z, 1.0}});
				//	//cameraPosition.setBufferData(std::vector<DirectX::SimpleMath::Vector3>{ cameraPos});
				//	time.setBufferData(std::vector<DirectX::SimpleMath::Vector4>{ {1,0,0,0}});
				//	cameraPosition.setPixelShaderBuffer();
				//	time.setBuffer();
				//}
				opaqueInstance->render(cameraPos);
			}
		}

		bool findIntersection(const ray& r, Instance*& instance, Intersection& intersection)
		{
			bool returnResult{};
			/*Intersection intersection;*/
			//intersection.reset();

			for (auto& triangleOctree : triangleOctrees)
			{
				if (triangleOctree->intersect(r, intersection))
				{
					instance = triangleOctree->m_instance;
					returnResult = true;
				}
			}

			return returnResult;
			
		}


		/// <param name="instance">instance, which is contained in opaqueInstance</param>
		void updateOpaqueInstanceBuffer(Instance* instance)
		{
			triangleOctreeInstances.at(instance)->needToUpdateInstanceBuffer();
		}




		/// <param name="instances">instance may be change during dragging</param>
		void addInstances(uint32_t opaqueInstanceID,
			const std::shared_ptr<Model>& model, size_t meshIndex,
			const std::shared_ptr<OpaqueInstances::Material>& material,
			const std::vector<OpaqueInstances::Instance>& instances)
		{
			for (Instance* instance : opaqueInstances.at(opaqueInstanceID)->addInstances(model, meshIndex, material, instances))
			{
				if (model->getMesh(meshIndex).GetName() != "cube1")
				{
					TriangleOctree triangleOctree;
					triangleOctree.initialize(model->getMesh(meshIndex), instance);
					triangleOctrees.emplace_back(std::make_shared<TriangleOctree>(std::move(triangleOctree)));
					triangleOctreeInstances[instance] = opaqueInstances.at(opaqueInstanceID).get();
				}

				
			}
		}

		/// <summary>
		/// creates new Opaque Instance
		/// </summary>
		/// <returns>ID to access to created opaque instance</returns>
		uint32_t createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName)
		{
			std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
			opaqueInstance->setVertexShader(vertexShaderFileName);
			opaqueInstance->setPixelShader(pixelShaderFileName);
			opaqueInstances.push_back(opaqueInstance);
			return opaqueInstances.size() - 1;
		}

		//void setVertexShader(const std::shared_ptr<OpaqueInstances>& opaqueInstance, const std::wstring& fileName)
		//{
		//	if (opaqueInstance.get() == normalOpaqueInstances.get())
		//	{
				//normalOpaqueInstances->setVertexShader(fileName);
		//	}
		//	else if (opaqueInstance.get() == hologramOpaqueInstances.get())
		//	{
		//		hologramOpaqueInstances->setVertexShader(fileName);
		//	}
		//}

		//void setPixelShader(const std::shared_ptr<OpaqueInstances>& opaqueInstance,const std::wstring& fileName)
		//{
		//	if (opaqueInstance.get() == normalOpaqueInstances.get())
		//	{
		//		normalOpaqueInstances->setPixelShader(fileName);
		//	}
		//	else if (opaqueInstance.get() == hologramOpaqueInstances.get())
		//	{
		//		hologramOpaqueInstances->setPixelShader(fileName);
		//	}
		//}

		ConstantBuffer<DirectX::SimpleMath::Vector4> cameraPosition;
		ConstantBuffer<DirectX::SimpleMath::Vector4> time;

	private:
		MeshSystem()
		{
			cameraPosition.initBuffer(PER_VIEW_PIXEL_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
			time.initBuffer(PER_DRAW_HOLOGRAM_SHADER, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);
		};
		std::vector<std::shared_ptr<OpaqueInstances>> opaqueInstances{};
		std::vector<std::shared_ptr<TriangleOctree>> triangleOctrees{};

		//used to update instance buffer during dragging
		std::unordered_map<Instance*, OpaqueInstances*> triangleOctreeInstances{};


		//ConstantBuffer<float> time;
		//std::shared_ptr<OpaqueInstances> normalOpaqueInstances;
		//std::shared_ptr<OpaqueInstances> hologramOpaqueInstances;
	};

}
