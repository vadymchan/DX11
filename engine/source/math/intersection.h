#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{
	struct Intersection
	{
		glm::vec3 point{};
		glm::vec3 normal{};
		glm::vec3 dir{};
		float t{};
	};
}