#pragma once
#include "../../MeshSystem/OpaqueInstances.h"
#include "../../D3D/D3D.h"

namespace engine::DX
{

	class MeshMover
	{
	public:

		static MeshMover& getInstance()
		{
			static MeshMover instance;
			return instance;
		}

		void setInstance(Instance instance)
		{
			this->instance = instance;
		}

		void setIntersectionPoint(const float4& intersectionPoint)
		{
			this->intersectionPoint = intersectionPoint;
		}


		void moveMesh(const float3& offset)
		{

			TransformSystem::getInstance().getTransform(instance.worldMatrixID)._14 += offset.x;
			TransformSystem::getInstance().getTransform(instance.worldMatrixID)._24 += offset.y;
			TransformSystem::getInstance().getTransform(instance.worldMatrixID)._34 += offset.z;

			intersectionPoint.x += offset.x;
			intersectionPoint.y += offset.y;
			intersectionPoint.z += offset.z;

			MeshSystem::getInstance().updateOpaqueInstanceBuffer(instance);
		}

		
		Instance getMat()
		{
			return instance;
		}

		const float4& getIntersectionPoint() const
		{
			return intersectionPoint;
		}

		MeshMover(const MeshMover&) = delete;
		MeshMover operator=(const MeshMover&) = delete;


	private:
		MeshMover()
		{
		}

		Instance instance;
		float4 intersectionPoint;

	};
}