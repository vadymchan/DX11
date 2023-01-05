#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{

	class DirectionalLight
	{
	public:
		DirectionalLight(glm::vec3 direction) : direction(direction) {}
		glm::vec3 getDirection() const { return direction; }
	private:
		glm::vec3 direction;
	};



}



