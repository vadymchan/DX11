#include "Scene.h"

void Engine::Scene::render(const Window& window, std::shared_ptr <Camera> camera, const glm::vec2& mousePoint)
{
	uint32_t numThreads = std::max(1u,
		std::max(Engine::ParallelExecutor::MAX_THREADS > 4u ? Engine::ParallelExecutor::MAX_THREADS - 4u : 1u,
			Engine::ParallelExecutor::HALF_THREADS));

	Engine::ParallelExecutor executor{ numThreads };

	glm::mat4 IPV = camera->getIPV();

	int bufferPointX = mousePoint.x == -1 ? -1 : mousePoint.x / window.windowBufferRatio;
	int bufferPointY = mousePoint.y == -1 ? -1 : mousePoint.y / window.windowBufferRatio;


	int windowHeight = window.GetBufferHeight();
	int windowWidth = window.GetBufferWidth();
	float verticalOffset{ 1 / (2.0f * windowHeight) };
	float horizontalOffset{ 1 / (2.0f * windowWidth) };
	float xNdcCoefficient{ 2.0f / windowWidth }; // x * 2 / windowWidth - 1 - to get NDC coordinates
	
	auto func = [this, &window, windowHeight, windowWidth, verticalOffset, horizontalOffset, xNdcCoefficient, &camera, &IPV, bufferPointX, bufferPointY](uint32_t threadIndex, uint32_t taskIndex)
	{

		computeColor(window, windowHeight, windowWidth, verticalOffset, horizontalOffset, xNdcCoefficient, IPV, bufferPointX, bufferPointY, taskIndex % window.GetBufferWidth(), taskIndex / window.GetBufferWidth(), camera->position());
	};

	executor.execute(func, window.GetBufferHeight() * window.GetBufferWidth(), 100);
	
	return;
}

void Engine::Scene::computeColor(const Window& window, float windowHeight, float windowWidth, float verticalOffset, float horizontalOffset, float xNdcCoefficient, const glm::mat4& IPV, int xCapture, int yCapture, int x, int y, const glm::vec3& cameraPosition)
{
	bool captureObjRay{ xCapture == x && yCapture == y };

	// render image
	// change to NDC
	// [0;480] -> [-1;1]
	// ([-1;1] + 1) / 2 * 480 = [0;480]
	float yNdc = ((windowHeight - y) * 2.0f / windowHeight - 1) - verticalOffset;
	float xNdc = (x * xNdcCoefficient - 1) + horizontalOffset;


	// screen to ndc
	glm::vec4 nearPointNDC(xNdc, yNdc, 1.0f, 1.0f);
	glm::vec4 farPointNDC(xNdc, yNdc, 0.0f, 1.0f);

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
	uint32_t pixel = castRayColorToUint(rayColor);
	window.drawPixel(x, y, pixel);

}


uint32_t Engine::Scene::castRayColorToUint(const glm::vec3& rayColor)
{
	auto r = rayColor.r * 255;
	auto g = rayColor.g * 255;
	auto b = rayColor.b * 255;

	uint32_t red = static_cast<uint32_t>(r) << 16;
	uint32_t green = static_cast<uint32_t>(g) << 8;
	uint32_t blue = static_cast<uint32_t>(b);

	return red | green | blue;
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
			mover->cameraToNearPlane = lenToNearPlane;
			//mover->rayNearPlaneRatio = (lenToNearPlane + intersection.t) / lenToNearPlane;
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

	//float aspectRatio = (lenToNearPlane + intersection.t)  / lenToNearPlane; 
	float aspectRatio = intersection.t; 
	float rayToObjIntersection = intersection.t;

	switch (obj.type)
	{
	case IntersectedType::Sphere:
		mover.reset(new SphereMover(static_cast<Sphere*>(obj.object), aspectRatio, lenToNearPlane, rayToObjIntersection, intersection.point));
		break;
	case IntersectedType::Plane:
		mover.reset(new PlaneMover(static_cast<Plane*>(obj.object), aspectRatio, lenToNearPlane, rayToObjIntersection, intersection.point));
		break;
	case IntersectedType::Triangle:
		mover.reset(new TriangleMover(static_cast<Triangle*>(obj.object), aspectRatio, lenToNearPlane, rayToObjIntersection, intersection.point));
		break;
	case IntersectedType::Cube:
		mover.reset(new MeshMover(static_cast<ColorCube*>(obj.object)->getPtrMathMesh(), aspectRatio));
		break;
	case IntersectedType::PointLight:
		mover.reset(new SphereMover(static_cast<PointLight*>(obj.object)->GetPtrSphere(), aspectRatio, lenToNearPlane, rayToObjIntersection, intersection.point));
		break;
	case IntersectedType::SpotLight:
		mover.reset(new SphereMover(static_cast<SpotLight*>(obj.object)->GetPtrSphere(), aspectRatio));
		break;
	case IntersectedType::Num:
		mover.reset(nullptr);
		return;
		break;
	}
}


void Engine::Scene::getObjectColor(const Intersection& intersection, const ObjRef& objRef, glm::vec3& color)
{
	
	Material mat;
	const float shineCoef{ 128 };

	switch (objRef.type)
	{
	case IntersectedType::Sphere:
		mat = static_cast<ColoredSphere*>(objRef.object)->material;
		break;
	case IntersectedType::Plane:
		mat = static_cast<ColoredPlane*>(objRef.object)->material;
		break;
	case IntersectedType::Triangle:
		mat = static_cast<ColoredTriangle*>(objRef.object)->material;
		break;
	case IntersectedType::Cube:
		mat = reinterpret_cast<const ColorCube*>(objRef.object)->material;
		break;
	case IntersectedType::PointLight:
		color = static_cast<ColorPointLight*>(objRef.object)->getColor();
		return;
		break;
	case IntersectedType::SpotLight:
		color = reinterpret_cast<ColorSpotLight*>(objRef.object)->getColor();
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
		Intersection shadowIntersection{ intersection };
		ObjRef shadowObjRef{ objRef };

		ray lDir{ directionLight.getRayToLight(intersection)};
		bool intersectedObject{ findIntersection(lDir, shadowIntersection, shadowObjRef)
			&& shadowObjRef.type != IntersectedType::PointLight
			&& shadowObjRef.type != IntersectedType::SpotLight };
		color += directionLight.computeFragmentColor(intersection, mat, lDir, shineCoef, intersectedObject);
	}


	//point light
	for (const auto& pointLight : pointLights)
	{
		Intersection shadowIntersection{ intersection };
		ObjRef shadowObjRef{ objRef };
		ray lDir{ pointLight.getRayToLight(shadowIntersection) };
		bool intersectedObject{ findIntersection(lDir, shadowIntersection, shadowObjRef)
			&& shadowObjRef.type != IntersectedType::PointLight 
			&& shadowObjRef.type != IntersectedType::SpotLight };
		color += pointLight.computeFragmentColor(intersection, mat, lDir, shineCoef, intersectedObject);

	}

	

	//flash light
	for (const auto& spotLight : spotLights)
	{
		Intersection shadowIntersection{ intersection };
		ObjRef shadowObjRef{ objRef };
		ray lDir{ spotLight.getRayToLight(shadowIntersection) };
		bool intersectedObject{ findIntersection(lDir, shadowIntersection, shadowObjRef)
			&& shadowObjRef.type != IntersectedType::PointLight
			&& shadowObjRef.type != IntersectedType::SpotLight };
		color += spotLight.computeFragmentColor(intersection, mat, lDir, shineCoef, intersectedObject);

	}

	color = glm::clamp(color, 0.0f, 1.0f);
}

bool Engine::Scene::findIntersection(const ray& r, Intersection& intersection, ObjRef& obj)
{
	bool hitObject{ false };


	//sphere hit
	for (auto& sphere : matSpheres)
	{
		if (sphere.hit(r, intersection))
		{
			obj.type = IntersectedType::Sphere;
			obj.object = &sphere;
			hitObject = true;
		}

	}

	//plane hit
	for (auto& plane : matPlanes)
	{
		if (plane.hit(r, intersection))
		{
			obj.type = IntersectedType::Plane;
			obj.object = &plane;
			hitObject = true;
		}
	}


	//triangle hit 
	for (auto& triangle : matTriangles)
	{
		if (triangle.hit(r, intersection))
		{	
			obj.type = IntersectedType::Triangle;
			obj.object = &triangle;
			hitObject = true;
		}
	}

	//mesh hit - as example
	//for (auto& mesh : matMeshes)
	//{
	//	glm::vec3 triangleN{};
	//	if (mesh.hit(r, intersection.t , triangleN))
	//	{
	//		intersection.t = intersection.t ;
	//		intersection.normal = triangleN;
	//		intersection.point = r.at(intersection.t ) + intersection.normal * bias;
	//		intersection.dir = r.direction();
	//		obj.type = IntersectedType::Cube;
	//		obj.object = &mesh;
	//		hitObject = true;
	//	}
	//}

	//triangle octree hit
	for (auto& tOctree : triangleOctrees)
	{
		if (tOctree.intersect(r, intersection))
		{
			obj.object = tOctree.m_mesh;
			obj.type = IntersectedType::Cube;
			hitObject = true;
		}
	}

	//pointlight hit
	for (auto& pointLight : pointLights)
	{
		if (pointLight.hit(r, intersection))
		{
			obj.type = IntersectedType::PointLight;
			obj.object = &pointLight;
			hitObject = true;
		}
	}

	//spotlight hit
	for (auto& spotLight : spotLights)
	{
		if (spotLight.hit(r, intersection))
		{
			obj.type = IntersectedType::SpotLight;
			obj.object = &spotLight;
			hitObject = true;
		}
	}

	return hitObject;
}

