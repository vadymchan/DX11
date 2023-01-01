#pragma once
#include "../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{

	class Light
	{
	public:
		Light() {}
		Light(glm::vec3 direction) : direction(direction) {}
		~Light() = default;

		glm::vec3 getDirection() { return direction; }

	private:

		glm::vec3 direction;
	};



}


