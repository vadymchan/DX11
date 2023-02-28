#include "MeshSystem.h"


namespace engine::DX
{




	void MeshSystem::render(const DirectX::SimpleMath::Vector3& cameraPos)
	{
		for (const auto& opaqueInstance : opaqueInstances)
		{
			opaqueInstance->render(cameraPos);
		}
	}

	/// <param name="instance">instance, which is contained in opaqueInstance</param>
	bool MeshSystem::findIntersection(const ray& r, Instance*& instance, Intersection& intersection)
	{
		bool returnResult{};

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

	/// <param name="instances">instance may be change during dragging</param>
	void MeshSystem::updateOpaqueInstanceBuffer(Instance* instance)
	{
		triangleOctreeInstances.at(instance)->needToUpdateInstanceBuffer();
	}

	void MeshSystem::addInstances(uint32_t opaqueInstanceID, const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<OpaqueInstances::Instance>& instances)
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
	uint32_t MeshSystem::createOpaqueInstance(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName)
	{
		std::shared_ptr<OpaqueInstances> opaqueInstance = std::make_shared<OpaqueInstances>();
		opaqueInstance->setVertexShader(vertexShaderFileName);
		opaqueInstance->setPixelShader(pixelShaderFileName);
		opaqueInstances.push_back(opaqueInstance);
		return opaqueInstances.size() - 1;
	}

}