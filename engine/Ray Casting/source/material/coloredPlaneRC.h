#pragma once

#include "../math/planeRC.h"
#include "materialRC.h"


namespace RC::engine
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
