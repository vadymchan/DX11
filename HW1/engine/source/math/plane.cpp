#include "plane.h"

bool Engine::Plane::hit(ray& r, float far, float& t_cur)
{


	float denom = glm::dot(r.direction(), N);

	if (denom > 0) // if n is in the same dirction as ray
	{
		N = -N;
	}

	if (denom != 0) //if plane and ray is parallel
	{
		//float t = -(glm::dot(r.origin(), n) + d) / denom;
		float t = glm::dot(D - r.origin(), N) / denom;

		if (t <= far && t < t_cur && t >= 0)
		{
			const float bias = 1e-4;
			r.changeOrigin(r.origin() + t * r.direction() + N * bias);
			t_cur = t;
			return true;
		}
	}


	return false;
}
