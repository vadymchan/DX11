#pragma once

#include "../math/plane.h"
#include "material.h"

namespace engine
{

	class ColoredPlane : public math::Plane
	{

	public:
		Material material;
		ColoredPlane(const Material& material, const Plane& plane)
		: material (material)
		, Plane (plane)
		{
		}

	};

}
