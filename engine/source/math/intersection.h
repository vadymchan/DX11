#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{
	struct Intersection
	{
		glm::vec3 point;
		glm::vec3 normal;
		glm::vec3 dir;
		float t;
		const float bias{1e-3};
		bool isValid() const { return std::isfinite(t); }
		void reset() { t = std::numeric_limits<float>::infinity(); }
	};
}