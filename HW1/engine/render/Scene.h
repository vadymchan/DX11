#pragma once
#include "../source/math/vec3.h"
#include "../source/math/sphere.h"
#include "../source/math/light.h"
#include "../source/math/plane.h"
#include "../windows/Window.h"
#include "Camera.h"
#include <limits>



namespace Engine
{


class Scene
{
public:
	Scene() {}
	Scene(const Sphere& sphere, const Light& light, const Plane& plane) 
		: sphere(sphere)
		, light(light)
		, plane(plane)
	{}

	void render(Window& window, Camera& camera);

	glm::vec3 castRay(ray& ray,float farPlane, float bgColor);

	void ChangeSpherePosition(glm::vec3 spherePosition);

	Sphere sphere;

private:
	Light light;
	Plane plane;
};

}

