#pragma once
#include "../math/sphere.h"
#include "material.h"



namespace Engine
{

	class ColoredSphere : public Sphere
	{

	public:

		Material material;

		ColoredSphere(Material material, Sphere sphere)
			: material(material)
			, Sphere(sphere)
		{}
	};

}


