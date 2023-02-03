#pragma once


#include "../../dependencies/glm/glm/ext/vector_float3.hpp"
#include "../../dependencies/glm/glm/geometric.hpp"
#include "ray.h"
#include "intersection.h"


namespace engine
{
	namespace math
	{
		class Plane
		{
		public:

			Plane(const glm::vec3& normal, const glm::vec3& distance)
				: normal(glm::normalize(normal))
				, distance(distance)
			{}

			const glm::vec3& getNormal() const { return normal; }
			const glm::vec3& getDistance() const { return distance; }

			bool hit(const ray& r, Intersection& near);

		private:
			glm::vec3 normal;
			glm::vec3 distance;
		};
	}




}

