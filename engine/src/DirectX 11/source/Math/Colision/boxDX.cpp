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

	if (tNear > tFar || near_t < tNear || tFar < 0)
	{
		return false;
	}

	near_t = tNear;
	return true;

}

