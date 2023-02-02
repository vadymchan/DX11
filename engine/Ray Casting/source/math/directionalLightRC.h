#pragma once
#include "../../dependencies/glm/glm/ext/vector_float3.hpp"

namespace engine::RC
{

	class DirectionalLight
	{
	public:
		DirectionalLight(const glm::vec3& direction) : direction(direction) {}
		glm::vec3 getDirection() const { return direction; }
	private:
		glm::vec3 direction;
	};



}



