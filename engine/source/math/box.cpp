#include "box.h"
#include <algorithm>

bool Engine::Box::hit(const ray& r, float& near_t) const
{
	glm::vec3 tMin = (min - r.origin()) / r.direction();
	glm::vec3 tMax = (max - r.origin()) / r.direction();
	glm::vec3 t1 = glm::min(tMin, tMax);
	glm::vec3 t2 = glm::max(tMin, tMax);
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
