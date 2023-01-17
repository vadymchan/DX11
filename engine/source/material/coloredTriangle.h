#pragma once
#include "../math/triangle.h"
#include "material.h"

namespace Engine
{
	class ColoredTriangle : public Triangle
	{
	public:
		Material material;
		ColoredTriangle(const Triangle& triangle, const Material& material)
			: Triangle(triangle)
			, material(material)
			 
		{}
	};

}