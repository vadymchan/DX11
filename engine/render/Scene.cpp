#include "Scene.h"



void Engine::Scene::render(Window& window, std::shared_ptr <Camera> camera)
{
	

	glm::mat4x4 IPV = glm::inverse(
		glm::perspective(glm::radians(camera->getFOV()), camera->getAspectRatio(), camera->getNearPlane(), camera->getFarPlane()) * 
		glm::lookAt(camera->getPosition(), camera->getPosition() - camera->getDirection(), camera->getCameraUp()));


	for (int y = window.GetBufferHeight() - 1; y >= 0; --y)
	{
		for (int x = 0; x < window.GetBufferWidth(); x++)
		{
			// render image

			// change to NDC
			// [0;480] -> [-1;1]
			// ([-1;1] + 1) / 2 * 480 = [0;480]
			float yNdc = ((window.GetBufferHeight() - y) * 2.0f / window.GetBufferHeight() - 1) - 1 / (2.0f * window.GetBufferHeight());
			float xNdc = (x * 2.0f / window.GetBufferWidth() - 1) + 1 / (2.0f * window.GetBufferWidth());

			// screen to ndc
			glm::vec4 nearPointNDC(xNdc, yNdc, -1.0f, 1.0f);
			glm::vec4 farPointNDC(xNdc, yNdc, 1.0f, 1.0f);

			//ndc to perspective to view to world
			glm::vec4 nearPointWorld = IPV * nearPointNDC;
			glm::vec4 farPointWorld = IPV * farPointNDC;
			nearPointWorld /= nearPointWorld.w;
			farPointWorld /= farPointWorld.w;

			ray initRay = ray(nearPointWorld, glm::normalize(farPointWorld - nearPointWorld));
			glm::vec3 rayColor = castRay(initRay, glm::length(farPointWorld - nearPointWorld), yNdc);

			auto r = rayColor.r * 255;
			auto g = rayColor.g * 255;
			auto b = rayColor.b * 255;

			uint32_t red = static_cast<uint32_t>(r) << 16;
			uint32_t green = static_cast<uint32_t>(g) << 8;
			uint32_t blue = static_cast<uint32_t>(b);

			uint32_t pixel = red + green + blue;

			window.drawPixel(x, y, pixel);
		}
	}


	return;
}





glm::vec3 Engine::Scene::castRay(ray& ray, float farPlane, float bgColorCoef)
{
	glm::vec3 finalColor = glm::vec3(0);
	Intersection intersection;
	ObjRef obj;

	if (findIntersection(ray, farPlane, intersection, obj))
	{
		getObjectColor(intersection, obj, finalColor);
	}
	else
	{
		glm::vec3 backgroundColor = (1 - (bgColorCoef + 1) / 2) * glm::vec3(1.0, 0.6, 0.0) + (bgColorCoef + 1) / 2 * glm::vec3(0.4, 0.6, 1.0);
		finalColor = backgroundColor;
	}

	return finalColor;
}


void Engine::Scene::getObjectColor(const Intersection& intersection, const ObjRef& obj, glm::vec3& color)
{
	Intersection shadowIntersection{ intersection };
	ObjRef shadowObjRef{ obj };
	Material mat;
	const float shineCoef{ 128 };

	switch (obj.type)
	{
	case IntersectedType::Sphere:
		mat = reinterpret_cast<ColoredSphere*>(obj.object)->material;

		break;
	case IntersectedType::Plane:
		mat = reinterpret_cast<ColoredPlane*>(obj.object)->material;

		break;
	case IntersectedType::Num:

		return;
		break;

	}
	//lighting

	//direction light

		int c{};
	for (const auto& directionLight : directionLights)
	{
		ray lDir{ Engine::ray(intersection.point, -directionLight.getDirection()) };
		if (findIntersection(lDir, FLT_MAX, shadowIntersection, shadowObjRef))
		{
			
			color += mat.ambient * directionLight.getColor()  ;
			c++;
			if (c == 2)
			{
				c = 0;
			}
		}
		else
		{
			glm::vec3 vDir{ intersection.dir };
			glm::vec3 halfVec{ glm::normalize(lDir.direction() - vDir)};
			float diffCoef{ max(glm::dot(intersection.normal, lDir.direction()), 0.0001f) };
			glm::vec3 diffuse = diffCoef * mat.diffuse ;
			float specCoef{ max(glm::dot(halfVec, intersection.normal), 0.0001f) };
			glm::vec3 specular = mat.specular * std::pow(specCoef, mat.shininess * shineCoef);
			color += mat.ambient * directionLight.getColor() + diffuse * directionLight.getColor() + specular * directionLight.getColor();


		}
	}
	color = glm::clamp(color, 0.0f, 1.0f);
}


bool Engine::Scene::findIntersection(const ray& r, float closest_t, Intersection& intersection, ObjRef& obj)
{
	bool hitObject{ false };
	float bias{ 1e-4f };


	//sphere hit
	for (auto& sphere : matSpheres)
	{
		if (sphere.hit(r, closest_t))
		{

			intersection.t = closest_t;
			intersection.normal = glm::normalize(r.at(closest_t) - sphere.getCenter());
			intersection.point = r.at(closest_t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.type = IntersectedType::Sphere;
			obj.object = &sphere;
			hitObject = true;
		}

	}

	//plane hit
	for (auto& plane : matPlanes)
	{
		if (plane.hit(r, closest_t))
		{

			intersection.t = closest_t;
			intersection.normal = plane.getN();
			intersection.point = r.at(closest_t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.type = IntersectedType::Plane;
			obj.object = &plane;
			hitObject = true;
		}
	}
	return hitObject;
}

