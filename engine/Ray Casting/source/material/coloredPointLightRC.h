#pragma once
#include "../math/pointLightRC.h"


namespace RC::engine
{
	class ColorPointLight : public PointLight
	{
	public:
		ColorPointLight(const Sphere& sphere, const glm::vec3& lightColor)
			: PointLight(sphere)
			, lightColor(lightColor)
		{}

		glm::vec3 getColor() const { return lightColor; }

	private:
		glm::vec3 lightColor;
	};
}

