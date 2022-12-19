#include "sphere.h"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"




bool Engine::Sphere::hit(ray& r, float far, float& t_cur) const 
{
	glm::vec3 oc = r.origin() - center;
	//auto a = glm::length(r.direction()) * glm::length(r.direction());
	auto a = glm::dot(r.direction(), r.direction());
	auto half_b = glm::dot(oc, r.direction());
	auto c = glm::length(oc) * glm::length(oc) - radius * radius;

	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	auto t = (-half_b - sqrtd) / a;
	if (far < t || t_cur < t || t < 0)
	{
		t = (-half_b + sqrtd) / a;
		if (far < t || t_cur < t || t < 0)
		{
			return false;
		}
	}


	//remake ----

	//glm::vec3 oc = r.origin() - center;
	////auto a = r.direction().length(); //normalised - so 1
	//auto half_b = glm::dot(oc, r.direction());
	//auto c = oc.length() - radius * radius;
	//auto discriminant = half_b * half_b - c;
	//if (discriminant < 0)
	//	return false;

	//auto sqrtd = sqrt(discriminant);
	//auto root = (-half_b - sqrtd);

	t_cur = t;
	return true;

}

bool Engine::Sphere::shadowObject(const ray& r) const
{
	glm::vec3 oc = r.origin() - center;
	auto a = glm::dot(r.direction(), r.direction());
	auto half_b = glm::dot(oc, r.direction());
	auto c = glm::length(oc) * glm::length(oc) - radius * radius;

	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);




	auto t = (-half_b - sqrtd) / a;
	if (t < 0)
	{
		t = (-half_b + sqrtd) / a;
		if (t < 0)
		{
			return false;
		}
	}
	return true;
}
