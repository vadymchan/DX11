#pragma once


#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include "rayRC.h"


namespace engine::RC
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

	bool hit(const ray& r, float& near);

private:
	glm::vec3 normal;
	glm::vec3 distance;
};


}

