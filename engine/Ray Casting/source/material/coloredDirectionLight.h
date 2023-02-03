#pragma once
#include "../math/directionalLight.h"
#include "material.h"

namespace engine
{
	class ColorDirectionLight : public math::DirectionalLight
	{
	public:
		ColorDirectionLight(const glm::vec3& direction , const glm::vec3& lightColor)
			: DirectionalLight(direction)
			, lightColor(lightColor)
		{
		};

		const glm::vec3& getColor() const
		{
			return lightColor;
		}

	private:
		glm::vec3 lightColor;
	};
}

