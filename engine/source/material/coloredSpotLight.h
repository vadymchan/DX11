#pragma once
#include "../math/spotLight.h"
#include "ColorLight.h"
#include <iostream>
namespace Engine
{
	class ColorSpotLight : public SpotLight, public ColorLight
	{
	public:
		ColorSpotLight(const Sphere& sphere, 
			const glm::vec3& direction,
			float outerCutoff, 
			float innerCutoff,
			const glm::vec3& lightColor)
			: SpotLight (sphere, direction, outerCutoff, innerCutoff)
			, ColorLight (lightColor)
		{}


		ray getRayToLight(Intersection& intersection) const
		{
			glm::vec3 lightToFrag{ intersection.point - getPosition() };
			ray lDir{ ray(intersection.point, glm::normalize(-lightToFrag)) };
			intersection.t = glm::length(lightToFrag);
			return lDir;
		}

		glm::vec3 computeFragmentColor(const Intersection& intersection, const Material& objectMaterial, const Engine::ray& dirToLight, float shineCoef, bool intersectObj) const
		{
			if (intersectObj)
			{
				return objectMaterial.ambient * getColor();
			}
			else
			{
				glm::vec3 lightToFrag{ intersection.point - getPosition() };

				// angle between light direction from fragment and spotLight direction 
				float phi{ glm::dot(glm::normalize(lightToFrag), glm::normalize(getDirection())) };

				if (phi >= getCosOuterCutoff())
				{
					return computeBlinnPhong(intersection, objectMaterial, dirToLight, shineCoef, true, phi, getCosOuterCutoff(), getCosInnerCutoff());
				}
				else
				{
					return objectMaterial.ambient * getColor();
				}

			}
		}


	};
}
