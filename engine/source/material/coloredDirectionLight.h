#pragma once
#include "../math/directionalLight.h"
#include "material.h"
#include <algorithm>
#include "ColorLight.h"

namespace Engine
{
	class ColorDirectionLight : public DirectionalLight, public ColorLight
	{
	public:
		ColorDirectionLight(const glm::vec3& direction, const glm::vec3& lightColor)
			: DirectionalLight(glm::normalize(direction))
			, ColorLight(lightColor)
		{};

		const glm::vec3& computeFragmentColor(const Intersection& intersection, const Material& objectMaterial, const Engine::ray& dirToLight, float shineCoef, bool intersectObj) const 
		{
			if (intersectObj)
			{
				return objectMaterial.ambient * lightColor;
			}
			else
			{
				return computeBlinnPhong(intersection, objectMaterial, dirToLight, shineCoef);
			}

		}
	};
}

