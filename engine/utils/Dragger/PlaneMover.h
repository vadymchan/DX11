#pragma once
#include "IObjectMover.h"
#include "../../source/math/plane.h"


namespace Engine
{
	class PlaneMover : public IObjectMover
	{
	public:
		Plane* plane;


		PlaneMover(Plane* plane, float cameraToNearPlane, const glm::vec3& intersectionPoint)
		{
			this->plane = plane;
			this->cameraToNearPlane = cameraToNearPlane;
			this->intersectionPoint = intersectionPoint;
		}
		
		virtual void move(const glm::vec3& offset) override
		{
			plane->setDistance(plane->getDistance() + offset);

		}

	};
}