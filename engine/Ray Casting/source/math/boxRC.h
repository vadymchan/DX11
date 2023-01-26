#pragma once
#include "rayRC.h"
#include "../../dependencies/glm/glm/ext/vector_float3.hpp"
#include "../../dependencies/glm/glm/common.hpp"
#include <limits>

#pragma once



namespace RC::engine
{
	struct Box
	{
		glm::vec3 min;
		glm::vec3 max;

		static constexpr float Inf = std::numeric_limits<float>::infinity();
		static constexpr Box empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
		static constexpr Box unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

		glm::vec3 size() const { return max - min; }
		glm::vec3 center() const { return (min + max) / 2.f; }
		float radius() const { return size().length() / 2.f; }
		bool hit(const ray& r, float& near_t) const;

		

		void reset()
		{
			constexpr float maxf = std::numeric_limits<float>::max();
			min = { maxf , maxf , maxf };
			max = -min;
		}

		void expand(const Box& other)
		{
			min = glm::min(min, other.min);
			max = glm::max(max, other.max);
		}

		void expand(const glm::vec3& point)
		{
			min = glm::min(min, point);
			max = glm::max(max, point);
		}

		bool contains(const glm::vec3& P)
		{
			return
				min[0] <= P[0] && P[0] <= max[0] &&
				min[1] <= P[1] && P[1] <= max[1] &&
				min[2] <= P[2] && P[2] <= max[2];
		}
	};
}
