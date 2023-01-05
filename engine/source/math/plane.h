#pragma once


#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"
#include "ray.h"


namespace Engine
{


class Plane
{
public:

	Plane(glm::vec3 N, glm::vec3 D) 
		: N(glm::normalize(N)) 
		, D(D)
	{}

	glm::vec3 getN() const { return N; }
	glm::vec3 getD() const { return D;}

	bool hit(const ray& r, float& near);

private:
	glm::vec3 N;
	glm::vec3 D;
};


}

