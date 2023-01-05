#pragma once


#ifndef RAY_H
#define RAY_H



#include "../../dependencies/glm-0.9.9.9/glm/vec3.hpp"
#include "../../dependencies/glm-0.9.9.9/glm/gtc/matrix_transform.hpp"

namespace Engine
{


class ray
{
public:

	ray(const glm::vec3& origin, const glm::vec3& direction)
		: orig(origin), dir(glm::normalize(direction))
	{}

	glm::vec3 origin() const { return orig; } // A
	glm::vec3 direction() const { return dir; } // b

	void changeOrigin(glm::vec3 newOrigin) { orig = newOrigin; }
	void changeDirection(glm::vec3 newDirection) { dir = newDirection; }

	glm::vec3 at(float t) const
	{
		return orig + t * dir; //A + t * b  
	}
private:
	glm::vec3 orig;
	glm::vec3 dir;
};

#endif 

}