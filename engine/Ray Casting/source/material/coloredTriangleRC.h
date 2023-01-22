#pragma once
#include "../math/triangleRC.h"
#include "materialRC.h"

namespace RC::engine // rc - ray caster namespace
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