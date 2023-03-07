#pragma once
#include "MeshTriangleOctreeDX.h"


namespace engine::DX
{
	using Intersection = MeshTriangleOctree::Intersection;
	class ModelTriangleOctree
	{
	public:
		ModelTriangleOctree(const std::weak_ptr<Model>& model, const std::weak_ptr<Instance>& instance, const std::vector<size_t> meshIndices)
			: model{model}
			, instance{instance}
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
				 
				 

				//---------------------------------
				box.expand(meshBox);
				

				meshTriangleOctrees.at(i).initialize(mesh, this->instance);
			}


		}

		


		bool intersect(ray r, Intersection& intersection);

		const std::weak_ptr<Instance>& getInstance() const { return instance; }

	private:
		std::weak_ptr<Model> model;
		std::weak_ptr<Instance> instance;
		std::vector<MeshTriangleOctree> meshTriangleOctrees;
		Box box = Box::empty();
	};


}


