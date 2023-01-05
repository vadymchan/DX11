#pragma once
#include "../source/math/sphere.h"
#include "../source/material/coloredSphere.h"
#include "../source/math/directionalLight.h"
#include "../source/material/coloredDirectionLight.h"
#include "../source/math/plane.h"
#include "../source/material/coloredPlane.h"
#include "../windows/Window.h"
#include "../source/math/intersection.h"
#include "Camera.h"
#include <limits>
#include <algorithm>
#include <vector>


namespace Engine
{


	class Scene
	{
	protected:
		enum class IntersectedType { Sphere, Plane, Num };
		struct ObjRef
		{
			void* object{};
			IntersectedType type = IntersectedType::Num;
		};

	public:


		Scene(const std::vector<ColoredSphere>& spheres,
			const std::vector<ColoredPlane>& planes,
			const std::vector<ColorDirectionLight>& directionLights
		)
			: matSpheres(spheres)
			, matPlanes(planes)
			, directionLights(directionLights)
		{}


		void render(Window& window, std::shared_ptr<Camera> camera);
		bool findIntersection(const ray& r, float closest_t, Intersection& intersection, ObjRef& obj);
		void getObjectColor(const Intersection& intersection, const ObjRef& obj, glm::vec3& color);

		glm::vec3 castRay(ray& ray, float farPlane, float bgColor);

	private:
		std::vector<ColoredSphere> matSpheres;
		std::vector<ColoredPlane> matPlanes;
		std::vector<ColorDirectionLight> directionLights;
	};

}
