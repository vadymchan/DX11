#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace RC::engine
{
	struct Intersection
	{
		glm::vec3 point;
		glm::vec3 normal;
		glm::vec3 dir;
		float t;

		bool isValid() { return std::isfinite(t); }

		void reset() { t = std::numeric_limits<float>::infinity(); }
	};
}