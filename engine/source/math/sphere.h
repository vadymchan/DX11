#pragma once

#ifndef SPHERE_H
#define SPHERE_H


#include "ray.h"
#include "../../dependencies/glm-0.9.9.9/glm/vec3.hpp"
#include "intersection.h"

namespace engine
{
	namespace math
	{
		class Sphere
		{
		public:

			Sphere(const glm::vec3& cen, float r)
				: center(cen), radius(r) {};

			bool hit(const ray& r, Intersection& far);
			inline const glm::vec3& getCenter() const { return center; }
			inline float getRadius() const { return radius; }
			inline void setCenter(const glm::vec3& newCenter) { center = newCenter; }
		private:
			glm::vec3 center;
			float radius;

		};
	}

}

#endif