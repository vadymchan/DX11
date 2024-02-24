#pragma once
#include "../math/triangleRC.h"
#include "materialRC.h"

namespace engine::RC // rc - ray caster namespace
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