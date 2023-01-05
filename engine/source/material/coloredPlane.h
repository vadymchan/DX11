#pragma once

#include "../math/plane.h"
#include "material.h"

namespace Engine
{

	class ColoredPlane : public Plane
	{

	public:
		Material material;
		ColoredPlane(Material material, Plane plane)
		: material (material)
		, Plane (plane)
		{

		}

	};

}
