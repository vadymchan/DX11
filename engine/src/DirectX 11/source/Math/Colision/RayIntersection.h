#pragma once

#include "../../D3D/D3D.h"

namespace engine::DX
{


	struct RayIntersection
	{
		float3 point;
		float3 normal;
		float3 dir;
		float t;

		bool isValid() { return std::isfinite(t); }

		void reset() { t = std::numeric_limits<float>::infinity(); }
	};

}