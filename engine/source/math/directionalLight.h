#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace engine
{
	namespace math
	{
		class DirectionalLight
		{
		public:
			DirectionalLight(const glm::vec3& direction)
				: direction(direction)
			{}
			const glm::vec3& getDirection() const { return direction; }
		private:
			glm::vec3 direction;
		};
	}
}



