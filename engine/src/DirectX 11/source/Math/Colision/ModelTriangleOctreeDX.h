#pragma once
#include "MeshTriangleOctreeDX.h"


namespace engine::DX
{

	class ModelTriangleOctree
	{
	public:
		ModelTriangleOctree(const std::weak_ptr<Model>& model, const std::vector<size_t>& meshIndices)
			: m_model{model}
			, m_meshIndices{meshIndices}
		{
			meshTriangleOctrees.resize(meshIndices.size());
			for (size_t i = 0; i < meshIndices.size(); i++)
			{
				Mesh& mesh = model.lock().get()->getMesh(meshIndices.at(i));
				//---------------------------------
				//translate mesh box to model space
				Box meshBox = mesh.getBox();
				meshBox.min = float3::Transform(meshBox.min, mesh.getMeshToModelMat().at(0).Transpose());
				meshBox.max = float3::Transform(meshBox.max, mesh.getMeshToModelMat().at(0).Transpose());
				float3 temp = float3::Min(meshBox.min, meshBox.max);
				meshBox.max = float3::Max(meshBox.min, meshBox.max);
				meshBox.min = temp;
				 
				 meshTriangleOctrees.at(i).initialize(mesh);

				//---------------------------------
				box.expand(meshBox);
			}


		}

		bool alreadyInited(const std::shared_ptr<Model>& model, const std::vector<size_t>& meshIndices) const
		{
			if (model.get() != m_model.lock().get()
				|| meshIndices.size() != m_meshIndices.size() 
				|| !std::is_permutation(m_meshIndices.begin(), m_meshIndices.end(), meshIndices.begin()))
			{
				return false;
			}

			return true;
		}


		bool intersect(ray r, RayIntersection& intersection, TransformSystem::ID& instance);


	private:
		std::weak_ptr<Model> m_model;
		std::vector<size_t> m_meshIndices;
		std::vector<MeshTriangleOctree> meshTriangleOctrees;
		Box box = Box::empty();
	};


}


