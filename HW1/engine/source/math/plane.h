#pragma once


#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include "ray.h"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"

namespace Engine
{


class Plane
{
public:


	Plane() {}
	Plane(glm::vec3 N, glm::vec3 D) 
		: N(glm::normalize(N)) 
		, D(D)
	{}
	~Plane() = default;

	glm::vec3 getN()
	{
		return N; 
	}

	glm::vec3 getD()
	{
		return D;
	}

	bool hit(ray& r, float far, float& t_cur);

private:
	glm::vec3 N;
	glm::vec3 D;
};


}

