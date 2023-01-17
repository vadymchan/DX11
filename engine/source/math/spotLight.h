#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"

namespace Engine
{
	class SpotLight
	{
	public:
		SpotLight(const glm::vec3& position,
			const glm::vec3& direction,
			float cutOff)
			: position(position)
			, direction(direction)
			, cutOff(cutOff)
		{};


		void setPosition(glm::vec3 value) { position = value; }
		void setDirection(glm::vec3 value) { direction = value; }
		void setDirection(float value) { cutOff = value; }

		glm::vec3 getPosition() const { return position; }
		glm::vec3 getDirection() const { return direction; }
		float getCutOff() const { return cutOff; }

	private:
		glm::vec3 position;
		glm::vec3 direction;
		float cutOff;
	};
}

