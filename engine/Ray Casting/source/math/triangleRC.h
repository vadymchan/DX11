#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include "../../dependencies/glm-0.9.9.9/glm/geometric.hpp"
#include "rayRC.h"
#include <array>
#include <vector>

namespace RC::engine
{
	class Triangle
	{
	public:
		Triangle(const glm::vec3& vertex1, const glm::vec3& vertex2, const glm::vec3& vertex3, const glm::vec3& normal)
			: v{ vertex1, vertex2, vertex3 }
			, normal(normal)
		{
		};

		Triangle(const Triangle& other)
		{

			this->v = other.v;
			this->normal = other.normal;
		}

		bool hit(const ray& r, float& near_t);
		const glm::vec3& getN() const { return normal; }
		glm::vec3& operator[] (size_t i)
		{
			return v[i];
		}

	private:
		std::vector<glm::vec3> v;
		glm::vec3 normal;
	};
}


