#pragma once
#include "../math/sphere.h"
#include "material.h"



namespace engine
{

	class ColoredSphere : public math::Sphere
	{

	public:

		Material material;

		ColoredSphere(const Material& material, const Sphere& sphere)
			: material(material)
			, Sphere(sphere)
		{}
	};

}


