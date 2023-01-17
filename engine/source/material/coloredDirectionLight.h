#pragma once
#include "../math/directionalLight.h"
#include "material.h"

namespace Engine
{
	class ColorDirectionLight : public DirectionalLight
	{
	public:
		ColorDirectionLight(const glm::vec3& direction, const glm::vec3& lightColor)
			: DirectionalLight(glm::normalize(direction))
			, lightColor(lightColor)
		{};

		glm::vec3 getColor() const
		{
			return lightColor;
		}

	private:
		glm::vec3 lightColor;
	};
}

