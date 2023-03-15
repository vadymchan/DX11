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

		void setInstance(std::weak_ptr<Instance> instance)
		{
			this->instance = instance;
		}

		void setIntersectionPoint(const float4& intersectionPoint)
		{
			this->intersectionPoint = intersectionPoint;
		}


		void moveMesh(const float3& offset)
		{

			instance.lock()->toWorldMatrix._14 += offset.x;
			instance.lock()->toWorldMatrix._24 += offset.y;
			instance.lock()->toWorldMatrix._34 += offset.z;

			intersectionPoint.x += offset.x;
			intersectionPoint.y += offset.y;
			intersectionPoint.z += offset.z;

			MeshSystem::getInstance().updateOpaqueInstanceBuffer(instance);
		}

		
		std::weak_ptr<Instance> getMat()
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

		std::weak_ptr<Instance> instance;
		float4 intersectionPoint;

	};
}