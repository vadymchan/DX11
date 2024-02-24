#pragma once

#include "../../D3D/D3D.h"
#include <limits>

#pragma once



namespace engine::DX
{
	struct Box
	{
		float3 min;
		float3 max;

		static constexpr float Inf = std::numeric_limits<float>::infinity();
		static constexpr Box empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
		static constexpr Box unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

		float3 size() const { return max - min; }
		float3 center() const { return (min + max) / 2.f; }
		float radius() const { return size().Length() / 2.f; }
		bool hit(const ray& r, float& near_t) const;

		

		void reset()
		{
			constexpr float maxf = std::numeric_limits<float>::max();
			min = { maxf , maxf , maxf };
			max = -min;
		}

		void expand(const Box& other)
		{
			min = float3::Min(min, other.min);
			max = float3::Max(max, other.max);
		}

		void expand(const float3& point)
		{
			min = float3::Min(min, point);
			max = float3::Max(max, point);
		}

		bool contains(const float3& P)
		{
			return
				min.x <= P.x && P.x <= max.x &&
				min.y <= P.y && P.y <= max.y &&
				min.z <= P.z && P.z <= max.z;
		}
	};
}
