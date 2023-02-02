#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include <algorithm>
#include "../math/ray.h"
#include "material.h"
namespace Engine
{
	class ColorLight
	{
	public:
		ColorLight(const glm::vec3& lightColor) : lightColor(lightColor)
		{

		}

		const glm::vec3& getColor() const
		{
			return lightColor;
		}

		glm::vec3 computeBlinnPhong(const Intersection& intersection,
			const Material& objectMaterial,
			const ray& dirToLight,
			float shineCoef,
			bool spotLight = false,
			float phi = 0,
			float outerCutoff = 0,
			float innerCutoff = 0) const
		{
			glm::vec3 vDir{ intersection.dir };
			glm::vec3 halfVec{ glm::normalize(dirToLight.direction() - vDir) };

			float diffCoef{};
			float specCoef{};
			// camera is not in unlitted side of object
			if (glm::dot(vDir, intersection.normal) < 0)
			{
				diffCoef = std::max(glm::dot(intersection.normal, dirToLight.direction()), 0.0001f);
				specCoef = std::max(glm::dot(halfVec, intersection.normal), 0.0001f);
			}
			glm::vec3 diffuse = diffCoef * objectMaterial.diffuse;
			glm::vec3 specular = objectMaterial.specular * std::pow(specCoef, objectMaterial.shininess * shineCoef);

			if (spotLight)
			{

				float theta{ (phi - outerCutoff) / (innerCutoff - outerCutoff)};
				float intensity = glm::smoothstep(0.0f, 1.0f, theta);
				diffuse *= intensity;
				specular *= intensity;
			}

			return (objectMaterial.ambient + diffuse + specular) * lightColor;
		}



	protected:
		glm::vec3 lightColor;

	};

}
