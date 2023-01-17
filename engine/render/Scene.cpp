#include "Scene.h"

void Engine::Scene::render(const Window& window, std::shared_ptr <Camera> camera, const glm::vec2& mousePoint)
{
	//std::cout << matSpheres.at(0).getCenter().x << '\t' << matSpheres.at(0).getCenter().y << '\t' << matSpheres.at(0).getCenter().z << '\n';
	uint32_t numThreads = std::max(1u,
		std::max(Engine::ParallelExecutor::MAX_THREADS > 4u ? Engine::ParallelExecutor::MAX_THREADS - 4u : 1u,
			Engine::ParallelExecutor::HALF_THREADS));

	Engine::ParallelExecutor executor{ numThreads };

	glm::mat4 IPV = camera->getIPV();

	int bufferPointX = mousePoint.x == -1 ? -1 : mousePoint.x / window.windowBufferRatio;
	int bufferPointY = mousePoint.y == -1 ? -1 : mousePoint.y / window.windowBufferRatio;


	
	auto func = [this, &window, &camera, &IPV, bufferPointX, bufferPointY](uint32_t threadIndex, uint32_t taskIndex)
	{

		computeColor(window, IPV, bufferPointX, bufferPointY, taskIndex % window.GetBufferWidth(), taskIndex / window.GetBufferWidth(), camera->position());
	};

	executor.execute(func, window.GetBufferHeight() * window.GetBufferWidth(), 100);
	
	return;
}

void Engine::Scene::computeColor(const Window& window, const glm::mat4& IPV, int xCapture, int yCapture, int x, int y, const glm::vec3& cameraPosition)
{
	bool captureObjRay{ xCapture == x && yCapture == y };

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

	if (captureObjRay)
	{
		lenToNearPlane = glm::length(glm::vec3(nearPointWorld) - cameraPosition);
	}


	glm::vec3 rayColor = castRay(initRay, glm::length(farPointWorld - nearPointWorld), yNdc, captureObjRay);

	auto r = rayColor.r * 255;
	auto g = rayColor.g * 255;
	auto b = rayColor.b * 255;

	uint32_t red = static_cast<uint32_t>(r) << 16;
	uint32_t green = static_cast<uint32_t>(g) << 8;
	uint32_t blue = static_cast<uint32_t>(b);

	uint32_t pixel = red + green + blue;

	window.drawPixel(x, y, pixel);

}



glm::vec3 Engine::Scene::castRay(ray& ray, float farPlane, float bgColorCoef, bool captureObjRay)
{
	glm::vec3 finalColor = glm::vec3(0);
	Intersection intersection{};
	ObjRef obj{};
	intersection.t = farPlane;

	if (findIntersection(ray, intersection, obj))
	{
		if (captureObjRay && mover == nullptr)
		{
			setCapturedObj(obj, intersection);
		}
		else if (captureObjRay && mover != nullptr)
		{
			mover->rayNearPlaneRatio = (lenToNearPlane + intersection.t) / lenToNearPlane;
		}
		getObjectColor(intersection, obj, finalColor);
	}
	else
	{
		glm::vec3 backgroundColor = (1 - (bgColorCoef + 1) / 2) * glm::vec3(1.0, 0.6, 0.0) + (bgColorCoef + 1) / 2 * glm::vec3(0.4, 0.6, 1.0);
		finalColor = backgroundColor;
	}

	return finalColor;
}

void Engine::Scene::setCapturedObj(const ObjRef& obj, const Intersection& intersection)
{

	float aspectRatio = (lenToNearPlane + intersection.t)  / lenToNearPlane;

	switch (obj.type)
	{
	case IntersectedType::Sphere:
		mover.reset(new SphereMover(static_cast<Sphere*>(obj.object), aspectRatio));
		break;
	case IntersectedType::Plane:
		mover.reset(new PlaneMover(static_cast<Plane*>(obj.object), aspectRatio));
		break;
	case IntersectedType::Triangle:
		mover.reset(new TriangleMover(static_cast<Triangle*>(obj.object), aspectRatio));
		break;
	case IntersectedType::Mesh:
		mover.reset(new MeshMover(static_cast<ColorMesh*>(obj.object)->getPtrMathMesh(), aspectRatio));
		break;
	case IntersectedType::PointLight:
		mover.reset(new SphereMover(static_cast<PointLight*>(obj.object)->GetPtrSphere(), aspectRatio));
		break;
	case IntersectedType::Num:
		mover.reset(nullptr);
		return;
		break;
	}
}


void Engine::Scene::getObjectColor(const Intersection& intersection, const ObjRef& objRef, glm::vec3& color)
{
	Intersection shadowIntersection{ intersection };
	ObjRef shadowObjRef{ objRef };
	Material mat;
	const float shineCoef{ 128 };

	switch (objRef.type)
	{
	case IntersectedType::Sphere:
		mat = reinterpret_cast<ColoredSphere*>(objRef.object)->material;
		break;
	case IntersectedType::Plane:
		mat = reinterpret_cast<ColoredPlane*>(objRef.object)->material;
		break;
	case IntersectedType::Triangle:
		mat = reinterpret_cast<ColoredTriangle*>(objRef.object)->material;
		break;
	case IntersectedType::Mesh:
		mat = reinterpret_cast<const ColorMesh*>(objRef.object)->material;
		break;
	case IntersectedType::PointLight:
		color = reinterpret_cast<ColorPointLight*>(objRef.object)->getColor();
		return;
		break;
	case IntersectedType::Num:
		return;
		break;

	}



	//lighting

	//direction light

	for (const auto& directionLight : directionLights)
	{
		ray lDir{ Engine::ray(intersection.point, -directionLight.getDirection()) };
		if (findIntersection(lDir, shadowIntersection, shadowObjRef) 
			&& shadowObjRef.type != IntersectedType::PointLight)
		{

			color += mat.ambient * directionLight.getColor();

		}
		else
		{
			glm::vec3 vDir{ intersection.dir };
			glm::vec3 halfVec{ glm::normalize(lDir.direction() - vDir) };

			float diffCoef{};
			float specCoef{};
			// camera is not in unlitted side of object
			if (glm::dot(vDir, intersection.normal) < 0)
			{
				diffCoef = std::max(glm::dot(intersection.normal, lDir.direction()), 0.0001f);
				specCoef = std::max(glm::dot(halfVec, intersection.normal), 0.0001f);
			}
			glm::vec3 diffuse = diffCoef * mat.diffuse;

			glm::vec3 specular = mat.specular * std::pow(specCoef, mat.shininess * shineCoef);

			color += mat.ambient * directionLight.getColor() + diffuse * directionLight.getColor() + specular * directionLight.getColor();
		}
	}


	//point light
	for (const auto& pointLight : pointLights)
	{
		glm::vec3 fragTolight{ pointLight.GetPosition() - intersection.point };
		ray lDir{ Engine::ray(intersection.point, glm::normalize(fragTolight)) };
		shadowIntersection.t = glm::length(fragTolight);
		if (findIntersection(lDir, shadowIntersection, shadowObjRef)
			&& shadowObjRef.type != IntersectedType::PointLight)
		{
			color += mat.ambient * pointLight.getColor();
		}
		else
		{
			glm::vec3 vDir{ intersection.dir };
			glm::vec3 halfVec{ glm::normalize(lDir.direction() - vDir) };
			float diffCoef{};
			float specCoef{};
			if (glm::dot(vDir, intersection.normal ) < 0)
			{
				diffCoef = std::max(glm::dot(lDir.direction(), intersection.normal), 0.0001f);
				specCoef = std::max(glm::dot(halfVec, intersection.normal), 0.0001f);
			}
			glm::vec3 diffuse{ diffCoef * mat.diffuse };

			glm::vec3 specular{ std::pow(specCoef, mat.shininess * shineCoef) * mat.specular };

			color += mat.ambient * pointLight.getColor() + diffuse * pointLight.getColor() + specular * pointLight.getColor();
		}

	}

	//flash light
	for (const auto& flashLight : spotLights)
	{

		glm::vec3 lightToFrag{ intersection.point - flashLight.getPosition() };
		ray lDir{ ray(intersection.point, glm::normalize(-lightToFrag)) };
		shadowIntersection.t = glm::length(lightToFrag);

		if (findIntersection(lDir, shadowIntersection, shadowObjRef)
			&& shadowObjRef.type != IntersectedType::PointLight)
		{
			color += mat.ambient * flashLight.getColor();
		}
		else
		{
			// angle between light direction from fragment and flashlight direction 
			float phi{ std::max(glm::dot(glm::normalize(lightToFrag), glm::normalize(flashLight.getDirection())), 0.0f) };
			float halfCutOff{ glm::cos(glm::radians(flashLight.getCutOff() / 2)) };

			if (phi >= halfCutOff)
			{
				glm::vec3 vDir{ intersection.dir };
				glm::vec3 halfVec{ glm::normalize(lDir.direction() - vDir) };
				float diffCoef{  };
				float specCoef{};
				if (glm::dot(vDir, intersection.normal) < 0)
				{
					diffCoef = std::max(glm::dot(lDir.direction(), intersection.normal), 0.0001f);
					specCoef = std::max(glm::dot(halfVec, intersection.normal), 0.0001f);
				}
				glm::vec3 diffuse{ diffCoef * mat.diffuse };
				glm::vec3 specular{ std::pow(specCoef, mat.shininess * shineCoef) * mat.specular };

				color += mat.ambient * flashLight.getColor() + diffuse * flashLight.getColor() + specular * flashLight.getColor();
			}
			else
			{
				color += mat.ambient * flashLight.getColor();
			}

		}
	}

	color = glm::clamp(color, 0.0f, 1.0f);
}

bool Engine::Scene::findIntersection(const ray& r, Intersection& intersection, ObjRef& obj)
{
	bool hitObject{ false };
	float bias{ 1e-3 };

	//intersection.t = closest_t;

	//sphere hit
	for (auto& sphere : matSpheres)
	{
		if (sphere.hit(r, intersection.t))
		{
			intersection.normal = glm::normalize(r.at(intersection.t) - sphere.getCenter());
			intersection.point = r.at(intersection.t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.type = IntersectedType::Sphere;
			obj.object = &sphere;
			hitObject = true;
		}

	}

	//plane hit
	for (auto& plane : matPlanes)
	{
		if (plane.hit(r, intersection.t))
		{

			intersection.normal = plane.getN();
			intersection.point = r.at(intersection.t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.type = IntersectedType::Plane;
			obj.object = &plane;
			hitObject = true;
		}
	}


	//triangle hit 
	for (auto& triangle : matTriangles)
	{
		if (triangle.hit(r, intersection.t))
		{
			intersection.normal = triangle.getN();
			intersection.point = r.at(intersection.t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.type = IntersectedType::Triangle;
			obj.object = &triangle;
			hitObject = true;
		}
	}

	//mesh hit
	//for (auto& mesh : matMeshes)
	//{
	//	glm::vec3 triangleN{};
	//	if (mesh.hit(r, intersection.t , triangleN))
	//	{
	//		intersection.t = intersection.t ;
	//		intersection.normal = triangleN;
	//		intersection.point = r.at(intersection.t ) + intersection.normal * bias;
	//		intersection.dir = r.direction();
	//		obj.type = IntersectedType::Mesh;
	//		obj.object = &mesh;
	//		hitObject = true;
	//	}
	//}

	//triangle octree hit
	for (auto& tOctree : triangleOctrees)
	{

		if (tOctree.intersect(r, intersection))
		{
			intersection.point = r.at(intersection.t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.object = tOctree.m_mesh;
			obj.type = IntersectedType::Mesh;
			hitObject = true;
		}
	}







	//pointlight hit
	for (auto& pointLight : pointLights)
	{
		if (pointLight.hit(r, intersection.t))
		{
			intersection.normal = glm::normalize(r.at(intersection.t) - pointLight.GetPosition());
			intersection.point = r.at(intersection.t) + intersection.normal * bias;
			intersection.dir = r.direction();
			obj.type = IntersectedType::PointLight;
			obj.object = &pointLight;
			hitObject = true;
		}
	}

	return hitObject;
}

