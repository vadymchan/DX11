#include "Scene.h"



void engine::scene::Scene::render(Window& window, std::shared_ptr<Camera> camera)
{
	

	glm::mat4x4 IPV = glm::inverse(
		glm::perspective(glm::radians(camera->getFOV()), camera->getAspectRatio(), camera->getNearPlane(), camera->getFarPlane()) * 
		glm::lookAt(camera->getPosition(), camera->getPosition() - camera->getDirection(), camera->getCameraUp()));


	glm::vec4 nearPointNDC(0, 0, -1.0f, 1.0f);
	glm::vec4 farPointNDC(0, 0, 1.0f, 1.0f);

	int windowHeight = window.GetBufferHeight();
	int windowWidth = window.GetBufferWidth();
	float verticalOffset{ 1 / (2.0f * windowHeight) };
	float horizontalOffset{ 1 / (2.0f * windowWidth) };
	float xNdcCoefficient{ 2.0f / windowWidth }; // x * 2 / windowWidth - 1 - to get NDC coordinates

	for (int y = windowHeight - 1; y >= 0; --y)
	{
		float yNdc = ((windowHeight - y) * 2.0f / windowHeight - 1) - verticalOffset;
		nearPointNDC.y = farPointNDC.y = yNdc;
		for (int x = 0; x < windowWidth; x++)
		{
			// render image

			// change to NDC
			// [0;480] -> [-1;1]
			float xNdc = (x * xNdcCoefficient - 1) + horizontalOffset;
			// screen to ndc
			nearPointNDC.x = farPointNDC.x = xNdc;
			//ndc to perspective to view to world
			glm::vec4 nearPointWorld = IPV * nearPointNDC;
			glm::vec4 farPointWorld = IPV * farPointNDC;
			nearPointWorld /= nearPointWorld.w;
			farPointWorld /= farPointWorld.w;

			math::ray initRay = math::ray(nearPointWorld, glm::normalize(farPointWorld - nearPointWorld));
			glm::vec3 rayColor = castRay(initRay, glm::length(farPointWorld - nearPointWorld), yNdc);

			uint32_t pixel = castRayColorToUint(rayColor);

			window.drawPixel(x, y, pixel);
		}
	}

}


uint32_t engine::scene::Scene::castRayColorToUint(const glm::vec3& rayColor)
{
	auto r = rayColor.r * 255;
	auto g = rayColor.g * 255;
	auto b = rayColor.b * 255;

	uint32_t red = static_cast<uint32_t>(r) << 16;
	uint32_t green = static_cast<uint32_t>(g) << 8;
	uint32_t blue = static_cast<uint32_t>(b);

	return red | green | blue;
}


const glm::vec3& engine::scene::Scene::castRay(math::ray& ray, float farPlane, float bgColorCoef)
{
	glm::vec3 finalColor = glm::vec3(0);
	objIntersection->reset();
	if (findIntersection(ray, farPlane, objIntersection, intersectedObj))
	{
		getObjectColor(objIntersection, intersectedObj, finalColor);
	}
	else
	{
		glm::vec3 backgroundColor = (1 - (bgColorCoef + 1) / 2) * glm::vec3(1.0, 0.6, 0.0) + (bgColorCoef + 1) / 2 * glm::vec3(0.4, 0.6, 1.0);
		finalColor = backgroundColor;
	}

	return finalColor;
}


void engine::scene::Scene::getObjectColor(const std::shared_ptr<math::Intersection>& intersection, const std::shared_ptr<SceneObject>& obj, glm::vec3& color)
{
	std::shared_ptr<math::Intersection> shadowIntersection = std::make_shared<math::Intersection>(math::Intersection());
	shadowIntersection->t = intersection->t;
	shadowIntersection->dir = intersection->dir;
	std::shared_ptr<SceneObject> shadowObj ;
	Material mat = obj->getMaterial();
	const float shineCoef{ 128 };

	

	//lighting

	//direction light

	for (const auto& directionLight : directionLights)
	{
		math::ray lDir{ math::ray(intersection->point, -directionLight->getDirection()) };
		if (findIntersection(lDir, FLT_MAX, shadowIntersection, shadowObj))
		{
			color += mat.ambient * directionLight->getColor();
			shadowIntersection->t = intersection->t;
		}
		else
		{
			glm::vec3 vDir{ intersection->dir };
			glm::vec3 halfVec{ glm::normalize(lDir.direction() - vDir)};
			float diffCoef{ max(glm::dot(intersection->normal, lDir.direction()), 0.0001f) };
			glm::vec3 diffuse = diffCoef * mat.diffuse ;
			float specCoef{ max(glm::dot(halfVec, intersection->normal), 0.0001f) };
			glm::vec3 specular = mat.specular * std::pow(specCoef, mat.shininess * shineCoef);
			color += mat.ambient * directionLight->getColor() + diffuse * directionLight->getColor() + specular * directionLight->getColor();


		}
	}
	color = glm::clamp(color, 0.0f, 1.0f);
}


bool engine::scene::Scene::findIntersection(const math::ray& r, float closest_t,const std::shared_ptr<math::Intersection>& intersection, std::shared_ptr<SceneObject>& obj)
{
	bool hitObject{ false };

	for (auto& object : objects)
	{
		if(object->hit(r, intersection))
		{
			obj = object;
			hitObject = true;
		}
	}


	return hitObject;
}


void engine::scene::Scene::addSphere(const math::Sphere& sphere, const Material& material)
{
	objects.push_back(std::make_shared<SceneSphere>(SceneSphere(sphere, material)));
}

void engine::scene::Scene::addPlane(const math::Plane& plane, const Material& material)
{
	objects.push_back(std::make_shared<ScenePlane>(ScenePlane(plane, material)));
}

void engine::scene::Scene::addDirectionalLight(const glm::vec3& lightDirection, const glm::vec3& lightColor)
{
	directionLights.push_back(std::make_shared<ColorDirectionLight>(ColorDirectionLight(lightDirection, lightColor)));
}