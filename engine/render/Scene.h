#pragma once
#include "../source/math/sphere.h"
#include "../source/material/coloredSphere.h"
#include "../source/math/directionalLight.h"
#include "../source/material/coloredDirectionLight.h"
#include "../source/math/plane.h"
#include "../source/material/coloredPlane.h"
#include "../windows/Window.h"
#include "../source/math/intersection.h"
#include "SceneObject.h"
#include "ScenePlane.h"
#include "SceneSphere.h"
#include "Camera.h"
#include <limits>
#include <algorithm>
#include <vector>


namespace engine
{

	namespace scene
	{

		class Scene
		{

		public:

			void render(Window& window, std::shared_ptr<Camera> camera);
			uint32_t castRayColorToUint(const glm::vec3& rayColor);
			bool findIntersection(const math::ray& r, float closest_t, const std::shared_ptr<math::Intersection>& intersection, std::shared_ptr<SceneObject>& obj);
			void getObjectColor(const std::shared_ptr<math::Intersection>& intersection, const std::shared_ptr<SceneObject>& obj, glm::vec3& color);
			void addSphere(const math::Sphere& sphere, const Material& material);
			void addPlane(const math::Plane& plane, const Material& material);
			void addDirectionalLight(const glm::vec3& lightDirection, const glm::vec3& lightColor);


			const glm::vec3& castRay(math::ray& ray, float farPlane, float bgColor);

		private:
			std::shared_ptr<math::Intersection> objIntersection = std::make_shared<math::Intersection>(math::Intersection());
			std::shared_ptr<SceneObject> intersectedObj;
			std::vector<std::shared_ptr<SceneObject>> objects;
			std::vector<std::shared_ptr<ColorDirectionLight>> directionLights;
		};


		
	}
}
