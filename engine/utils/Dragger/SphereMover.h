#pragma once
#include "IObjectMover.h"
#include "../../source/math/sphere.h"


namespace Engine
{
	class SphereMover : public IObjectMover
	{
	public:
		Sphere* sphere;


		SphereMover(Sphere* sphere, float cameraToNearPlane, const glm::vec3& intersectionPoint)
		{
			this->sphere = sphere;
			this->cameraToNearPlane = cameraToNearPlane;
			this->intersectionPoint = intersectionPoint;

		}
		
		virtual void move(const glm::vec3& offset) override
		{
			sphere->setCenter(sphere->getCenter() + offset);

		}

	};
}