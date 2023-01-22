#pragma once
#include "../math/sphereRC.h"
#include "materialRC.h"

namespace RC::engine
{

	class ColoredSphere : public Sphere
	{

	public:

		Material material;

		ColoredSphere(const Material& material, const Sphere& sphere)
			: material(material)
			, Sphere(sphere)
		{}
	};

}


