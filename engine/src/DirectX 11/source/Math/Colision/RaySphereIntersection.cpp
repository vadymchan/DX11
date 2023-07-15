#include "RaySphereIntersection.h"

bool engine::DX::raySphereIntersect(const ray& r, const Sphere& s, RayIntersection& intersection)
{
	float3 m = r.position - s.center;

	float b = m.Dot(r.direction);
	float c = m.Dot(m) - s.radius * s.radius;

	// the ray is outside the sphere and the ray is pointing away from the sphere
	if (c > 0.0f && b > 0.0f) return false;

	float discriminant = b * b - c;

	// no real roots
	if (discriminant < 0.0f) return false;

	float t = -b - sqrt(discriminant);

	//intersection point lies behind the ray origin
	if (t < 0.0f) t = 0.0f;

	float3 intersectionPoint = r.position + r.direction * t;

	//filling the intersection
	intersection.point = intersectionPoint;
	float3 normalizedDirection = intersectionPoint - s.center;
	normalizedDirection.Normalize();
	intersection.normal = normalizedDirection;
	intersection.dir = r.direction;
	intersection.t = t;

	return true;
}
