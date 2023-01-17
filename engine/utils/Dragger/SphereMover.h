#pragma once
#include "IObjectMover.h"
#include "../../source/math/sphere.h"


namespace Engine
{
	class SphereMover : public IObjectMover
	{
	public:
		Sphere* sphere;


		SphereMover(Sphere* sphere, float aspectRatio)
		{
			this->sphere = sphere;
			rayNearPlaneRatio = aspectRatio;

		}
		
		virtual void move(const glm::vec3& offset) override
		{
			sphere->setCenter(sphere->getCenter() + offset);

		}

	};
}