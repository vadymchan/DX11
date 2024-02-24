#include "ModelTriangleOctreeDX.h"



bool engine::DX::ModelTriangleOctree::intersect(ray r, RayIntersection& intersection, TransformSystem::ID& instance)
{
	ray oldray = r; //for debug 
	ray modelRay = r;
    //transpose matrix since matrix is stored in column-space (but we need row-major for computation)
    float4x4 worldMatrix = TransformSystem::getInstance().getTransform(instance).Transpose();
    float4x4 invModelMatrix = worldMatrix.Invert();

    modelRay.position = float3::Transform(modelRay.position, invModelMatrix);
    modelRay.direction = float3::TransformNormal(modelRay.direction, invModelMatrix);
    //modelRay.direction.Normalize();


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

	//ray box intersection test
	/*intersection.t = boxT;
	intersection.point = oldray.position + oldray.direction * boxT;

	return true;*/
}
