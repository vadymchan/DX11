#include "Scene.h"



void Engine::Scene::render(Window& window, Camera& camera)
{

	// ndc to view matrix
	//glm::mat4x4 viewMatrixI = glm::inverse(glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection(), camera.getCameraUp()));

	// view to perspective matrix
	//glm::mat4x4 perspectiveMatrixI = glm::inverse(glm::perspective(glm::radians(camera.getFOV()), camera.getAspectRatio(), camera.getNearPlane(), camera.getFarPlane()));

	//glm::mat4x4 IPV = glm::inverse(glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getDirection(), camera.getCameraUp()) * glm::perspective(glm::radians(camera.getFOV()), camera.getAspectRatio(), camera.getNearPlane(), camera.getFarPlane()));
	glm::mat4x4 IPV = glm::inverse( glm::perspective(glm::radians(camera.getFOV()), camera.getAspectRatio(), camera.getNearPlane(), camera.getFarPlane()) 
	* glm::lookAt(camera.getPosition(), camera.getPosition() - camera.getDirection(), camera.getCameraUp()));


	for (int y = 0; y < window.GetBufferHeight(); y++)
	//for (int y = window.GetBufferHeight() - 1; y >= 0; --y)
	{
		for (int x = 0; x < window.GetBufferWidth(); x++)
		//for (int x = window.GetBufferHeight() - 1; x >= 0; --x)
		{
			// render image

			// change to NDC
			// [0;480] -> [-1;1]
			// ([-1;1] + 1) / 2 * 480 = [0;480]
			float yNdc = (window.GetBufferHeight() - y) * 2.0f / window.GetBufferHeight() - 1 - 1 / (2.0f * window.GetBufferHeight());
			float xNdc = x * 2.0f / window.GetBufferWidth() - 1 + 1 / (2.0f * window.GetBufferWidth());

			// screen to ndc
			glm::vec4 nearPointNDC(xNdc, yNdc, -1.0f, 1.0f);
			glm::vec4 farPointNDC(xNdc, yNdc, 1.0f, 1.0f);


			//ndc to view to perspective
			glm::vec4 nearPointWorld = IPV * nearPointNDC;
			glm::vec4 farPointWorld = IPV * farPointNDC;
			nearPointWorld /= nearPointWorld.w;
			farPointWorld /= farPointWorld.w;


			ray initRay = ray(nearPointWorld, glm::normalize(farPointWorld - nearPointWorld));
			glm::vec3 rayColor = castRay(initRay,farPointWorld.z, yNdc);

			auto r = rayColor.r * 255 * 1;
			auto g = rayColor.g * 255 * 1;
			auto b = rayColor.b * 255 * 1;

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
	//sphere hit

	
	float closest_t = FLT_MAX;
	glm::vec3 finalColor = glm::vec3(0);
	glm::vec3 sphereColor = glm::vec3(0);
	glm::vec3 planeColor = glm::vec3(0.2, 0.5, 0.2);
	glm::vec3 shadowColor = glm::vec3(0.2, 0.2, 0.2);
	glm::vec3 backgroundColor = (1 - (bgColorCoef + 1) / 2) * glm::vec3(1.0, 0.6, 0.0) + (bgColorCoef + 1) / 2 * glm::vec3(0.4, 0.6, 1.0);

	bool hitObject{ false };

	// for generating snow
	/*int snow = rand() % 1000;
	if (snow == 1)
	{
		return glm::vec3(1, 1, 1);
	}*/

	if (sphere.hit(ray, farPlane, closest_t))
	{
	
		sphereColor = glm::vec3(0.5) * (glm::normalize(ray.at(closest_t) - sphere.getCenter()) + glm::vec3(1));
		
		//this is for snow cape at top of the sphere
		//float snowCapeCoef = glm::normalize(ray.at(closest_t) - sphere.getCenter()).y;
		//if (glm::normalize(ray.at(closest_t) - sphere.getCenter()).y > 0.7f)
		//{
		//	//sphereColor = std::sqrt(snowCapeCoef) * glm::vec3(1.0, 1, 1) + (1- std::sqrt(snowCapeCoef)) * sphereColor;
		//	sphereColor = std::sqrt(snowCapeCoef) * glm::vec3(1.0, 1, 1);
		//}
		finalColor = sphereColor;
		hitObject = true;

	}

	//plane hit

	if (plane.hit(ray, farPlane, closest_t))
	{
		ray.changeDirection(-light.getDirection());
		if (sphere.shadowObject(ray))
		{
			finalColor = shadowColor + planeColor * 0.1f;
		}
		else
		{
			finalColor = planeColor;
		}
		hitObject = true;
	}



	if (!hitObject)
	{
		finalColor = backgroundColor;
	}

	return finalColor;
}

void Engine::Scene::ChangeSpherePosition(glm::vec3 spherePosition)
{
	sphere.setCenter(spherePosition);
}
