#pragma once
#include <glm/ext/vector_float3.hpp>

namespace engine::RC
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


		void setPosition(const glm::vec3& value) { position = value; }
		void setDirection(const glm::vec3& value) { direction = value; }
		void setDirection(float value) { cutOff = value; }

		const glm::vec3& getPosition() const { return position; }
		const glm::vec3& getDirection() const { return direction; }
		float getCutOff() const { return cutOff; }

	private:
		glm::vec3 position;
		glm::vec3 direction;
		float cutOff;
	};
}

