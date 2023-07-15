#pragma once

#include "../../D3D/D3D.h"
#include "../../Math/Colision/RayIntersection.h"


namespace engine::DX
{

	struct Sphere
	{
		float3 center;
		float radius;
	};

	bool raySphereIntersect(const ray& r, const Sphere& s, RayIntersection& intersection);

}