#pragma once
#include "IObjectMoverRC.h"
#include "../../source/math/planeRC.h"


namespace RC::engine
{
	class PlaneMover : public IObjectMover
	{
	public:
		Plane* plane;


		PlaneMover(Plane* plane, float aspectRatio)
		{
			this->plane = plane;
			rayNearPlaneRatio = aspectRatio;

		}
		
		virtual void move(const glm::vec3& offset) override
		{
			plane->setDistance(plane->getDistance() + offset);

		}

	};
}