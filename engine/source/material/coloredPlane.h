#pragma once

#include "../math/plane.h"
#include "material.h"

namespace Engine
{

	class ColoredPlane : public Plane
	{

	public:
		Material material;
		ColoredPlane(const  Material& material, const Plane& plane)
		: material (material)
		, Plane (plane)
		{

		}

	};

}
