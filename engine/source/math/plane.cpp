#include "plane.h"

bool Engine::Plane::hit(const ray& r, Intersection& near)
{
	glm::vec3 currentN{ normal };

	float denom = glm::dot(r.direction(), currentN);



	if (denom != 0) //if plane and ray is not parallel
	{
		float t = glm::dot(distance - r.origin(), currentN) / denom;

		if (t < near.t && t > 0) 
		{
			near.t = t;
			near.normal = getN();
			near.point = r.at(near.t) + near.normal * near.bias;
			near.dir = r.direction();
			return true;
		}
	}


	return false;
}
