#pragma once
#include "IObjectMover.h"
#include "../../source/math/sphere.h"


namespace Engine
{
	class SphereMover : public IObjectMover
	{
	public:
		Sphere* sphere;


		SphereMover(Sphere* sphere, float aspectRatio, float cameraToNearPlane, float rayToObjIntersection, const glm::vec3& intersectionPoint)
		{
			this->sphere = sphere;
			this->cameraToNearPlane = cameraToNearPlane;
			this->rayToObjIntersection = rayToObjIntersection;
			this->intersectionPoint = intersectionPoint;
			rayNearPlaneRatio = aspectRatio;

		}
		
		virtual void move(const glm::vec3& offset) override
		{
			sphere->setCenter(sphere->getCenter() + offset);

		}

	};
}