#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{
	class IObjectMover
	{
	public:
		float rayNearPlaneRatio;
		virtual void move(const glm::vec3& offset) = 0;
	};
}
