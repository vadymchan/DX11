#pragma once
#include "../math/pointLight.h"
#include "ColorLight.h"

namespace Engine
{
	class ColorPointLight : public PointLight, public ColorLight
	{
	public:
		ColorPointLight(const Sphere& sphere,const glm::vec3& lightColor)
			: PointLight(sphere)
			, ColorLight(lightColor)

		{}


		glm::vec3 computeFragmentColor(const Intersection& intersection, const Material& objectMaterial, const Engine::ray& dirToLight, float shineCoef, bool intersectObj) const
		{
			if (intersectObj)
			{
				return objectMaterial.ambient * getColor();
			}
			else
			{
				return computeBlinnPhong(intersection, objectMaterial, dirToLight, shineCoef);
			}
		}

	};
}

