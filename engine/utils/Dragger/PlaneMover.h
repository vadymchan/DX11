#pragma once
#include "IObjectMover.h"
#include "../../source/math/plane.h"


namespace Engine
{
	class PlaneMover : public IObjectMover
	{
	public:
		Plane* plane;


		PlaneMover(Plane* plane, float aspectRatio, float cameraToNearPlane, float rayToObjIntersection, const glm::vec3& intersectionPoint)
		{
			this->plane = plane;
			this->cameraToNearPlane = cameraToNearPlane;
			this->rayToObjIntersection = rayToObjIntersection;
			this->intersectionPoint = intersectionPoint;
			rayNearPlaneRatio = aspectRatio;

		}
		
		virtual void move(const glm::vec3& offset) override
		{
			plane->setDistance(plane->getDistance() + offset);

		}

	};
}