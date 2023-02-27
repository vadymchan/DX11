#include "boxDX.h"
#include <algorithm>

bool engine::DX::Box::hit(const ray& r, float& near_t) const
{
	

	float3 tMin = (min - r.position) / r.direction;
	float3 tMax = (max - r.position) / r.direction;
	float3 t1 = float3::Min(tMin, tMax);
	float3 t2 = float3::Max(tMin, tMax);
	float tNear = std::max(std::max(t1.x, t1.y), t1.z);
	float tFar = std::min(std::min(t2.x, t2.y), t2.z);

	if (tNear > tFar || near_t < tNear || tNear < 0)
	{
		if (tFar < 0 || near_t < tNear)
		{
			return false;
		}
		else
		{
			near_t = tFar;
			return true;
		}
		return false;
	}
	else
	{
		near_t = tNear;
		return true;
	}
	
}


//bool RayBoxIntersection(Ray ray, AABB box, float* tmin)
//{
//	*tmin = 0.0f;
//	float tmax = std::numeric_limits< float >::max();
//
//	for (int i = 0; i < 3; i++) {
//		if (std::abs(ray.direction[i]) < 0.00001f) {
//			// Ray is parallel to slab. No hit if origin not within slab
//			if (ray.origin[i] < box.min[i] || ray.origin[i] > box.max[i]) {
//				return false;
//			}
//		}
//		else {
//			// Compute intersection t value of ray with near and far plane of slab
//			float ood = 1.0f / ray.direction[i];
//			float t1 = (box.min[i] - ray.origin[i]) * ood;
//			float t2 = (box.max[i] - ray.origin[i]) * ood;
//
//			// Make t1 be intersection with near plane, t2 with far plane
//			if (t1 > t2) {
//				std::swap(t1, t2);
//			}
//
//			// Compute the intersection of slab intersection intervals
//			*tmin = glm::max(*tmin, t1);
//			tmax = glm::min(tmax, t2);
//
//			// Exit with no collision as soon as slabs don't overlap
//			if (*tmin > tmax) {
//				return false;
//			}
//		}
//	}
//
//	// Ray intersects all 3 slabs. Return tmin
//	return true;
//}