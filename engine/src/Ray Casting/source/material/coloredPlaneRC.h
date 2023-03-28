#pragma once

#include "../math/planeRC.h"
#include "materialRC.h"


namespace engine::RC
{

	class ColoredPlane : public Plane
	{

	public:
		Material material;
		ColoredPlane(const  Material& material, const Plane& plane)
			: material(material)
			, Plane(plane)
		{

		}

	};

}
