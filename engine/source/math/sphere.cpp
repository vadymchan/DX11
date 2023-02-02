#include "sphere.h"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"




bool Engine::Sphere::hit(const ray& r, Intersection& near)
{
	glm::vec3 oc = r.origin() - center;
	auto a = glm::length(r.direction()) * glm::length(r.direction());
	auto half_b = glm::dot(oc, r.direction());
	auto c = glm::length(oc) * glm::length(oc) - radius * radius;


	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	auto t = (-half_b - sqrtd) / a;
	if (near.t < t || t < 0)
	{
		t = (-half_b + sqrtd) / a;
		if (near.t < t || t < 0)
		{
			return false;
		}
	}
	near.t = t;
	near.normal = glm::normalize(r.getPointAt(near.t) - getCenter());
	near.point = r.getPointAt(near.t) + near.normal * near.bias;
	near.dir = r.direction();


	return true;

}


