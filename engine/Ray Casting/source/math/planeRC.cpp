#include "planeRC.h"

bool engine::RC::Plane::hit(const ray& r, float& near) 
{
	glm::vec3 currentN{ normal };

	float denom = glm::dot(r.direction(), currentN);

	//if (denom > 0) // if n is in the same direction as ray
	//{
	//	currentN = -currentN;
	//}

	if (denom != 0) //if plane and ray is not parallel
	{
		float t = glm::dot(distance - r.origin(), currentN) / denom;

		if (t < near && t > 0) // near <= or < ??
		{
			near = t;
			return true;
		}
	}


	return false;
}
