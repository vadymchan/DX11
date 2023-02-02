#pragma once
#include "../../dependencies/glm/glm/ext/vector_float3.hpp"
#include "sphereRC.h"

namespace engine::RC
{
	class PointLight
	{
	public:
		PointLight(const Sphere& sphere) 
			: sphere (sphere)
		{};

		glm::vec3 GetPosition() const
		{
			return sphere.getCenter();
		}

		float GetRadius() const
		{
			return sphere.getRadius();
		}

		Sphere* GetPtrSphere() 
		{
			return &sphere;
		}

		bool hit(const ray& r, float& near_t) 
		{
			return sphere.hit(r, near_t);
		}

	private:
		Sphere sphere;
	};
}


