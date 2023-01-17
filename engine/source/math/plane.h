#pragma once


#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"
#include "ray.h"


namespace Engine
{


class Plane
{
public:

	Plane(const glm::vec3& N, const glm::vec3& D)
		: N(glm::normalize(N)) 
		, D(D)
	{}

	glm::vec3 getN() const { return N; }
	glm::vec3 getD() const { return D; }

	void setD(const glm::vec3& value) { D = value; }

	bool hit(const ray& r, float& near);

private:
	glm::vec3 N;
	glm::vec3 D;
};


}

