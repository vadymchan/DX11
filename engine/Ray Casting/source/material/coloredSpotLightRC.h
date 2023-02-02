#pragma once
#include "../math/spotLightRC.h"

namespace engine::RC
{
	class ColorSpotLight : public SpotLight
	{
	public:
		ColorSpotLight(const glm::vec3& position,
			const glm::vec3& direction,
			float cutOff,
			const glm::vec3& lightColor)
			: SpotLight(position, direction, cutOff)
			, lightColor(lightColor)
		{}

		glm::vec3 getColor() const { return lightColor; }
	private:
		glm::vec3 lightColor;
	};
}