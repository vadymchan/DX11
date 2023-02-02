#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include "../math/ray.h"

namespace Engine
{

	class DirectionalLight
	{
	public:
		DirectionalLight(const glm::vec3& direction) : direction(direction) {}
		glm::vec3 getDirection() const { return direction; }
		ray getRayToLight(const Intersection& intersection) const{ return Engine::ray(intersection.point, -getDirection()); }
	private:
		glm::vec3 direction;
	};



}



