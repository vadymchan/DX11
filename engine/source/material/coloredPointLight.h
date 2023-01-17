#pragma once
#include "../math/pointLight.h"


namespace Engine
{
	class ColorPointLight : public PointLight
	{
	public:
		ColorPointLight(const Sphere& sphere,const glm::vec3& lightColor)
			: PointLight(sphere)
			, lightColor (lightColor)
		{}

		glm::vec3 getColor() const { return lightColor; }

	private:
		glm::vec3 lightColor;
	};
}

