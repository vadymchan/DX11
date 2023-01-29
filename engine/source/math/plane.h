#pragma once


#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"
#include "ray.h"
#include "intersection.h"


namespace Engine
{


class Plane
{
public:

	Plane(const glm::vec3& normal, const glm::vec3& distance)
		: normal(glm::normalize(normal))
		, distance(distance)
	{}

	glm::vec3 getNormal() const { return normal; }
	glm::vec3 getDistance() const { return distance; }

	void setDistance(const glm::vec3& value) { distance = value; }

	bool hit(const ray& r, Intersection& near);

private:
	glm::vec3 normal;
	glm::vec3 distance;
};


}

