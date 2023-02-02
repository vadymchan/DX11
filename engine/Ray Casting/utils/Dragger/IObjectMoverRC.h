#pragma once
#include "../../dependencies/glm/glm/ext/vector_float3.hpp"

namespace engine::RC
{
	class IObjectMover
	{
	public:
		float rayNearPlaneRatio;
		virtual void move(const glm::vec3& offset) = 0;
	};
}
