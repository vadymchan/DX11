#pragma once
#include "../math/directionalLight.h"
#include "material.h"

namespace Engine
{
	class ColorDirectionLight : public DirectionalLight
	{
	public:
		ColorDirectionLight(glm::vec3 direction ,glm::vec3 lightColor) 
			: DirectionalLight(direction)
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

