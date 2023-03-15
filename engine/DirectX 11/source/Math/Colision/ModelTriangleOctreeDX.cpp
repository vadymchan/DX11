#include "ModelTriangleOctreeDX.h"



bool engine::DX::ModelTriangleOctree::intersect(ray r, Intersection& intersection, const std::shared_ptr<Instance>& instance)
{
	/*ray oldray = r;*/ //for debug
	ray modelRay = r;
	float4x4 invModelMatrix = instance.get()->toWorldMatrix.Transpose().Invert();
	modelRay.position = float3::Transform(modelRay.position, invModelMatrix);
	modelRay.direction = float3::TransformNormal(modelRay.direction, invModelMatrix);
	modelRay.direction.Normalize();

	float boxT = intersection.t;
	if (!box.hit(modelRay, boxT))
		return false;

	bool hitAny{};
	for (auto& meshTriangleOctree : meshTriangleOctrees)
	{
		if (meshTriangleOctree.intersect(r, intersection, instance))
		{
			hitAny = true;
		}

	}

	return hitAny;
}
