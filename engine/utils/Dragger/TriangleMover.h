#pragma once
#include "IObjectMover.h"
#include "../../source/math/triangle.h"


namespace Engine
{
	class TriangleMover : public IObjectMover
	{
	public:
		Triangle* triangle;


		TriangleMover(Triangle* triangle, float aspectRatio)
		{
			this->triangle = triangle;
			rayNearPlaneRatio = aspectRatio;

		}
		
		virtual void move(const glm::vec3& offset) override
		{
			triangle->operator[](0) = triangle->operator[](0) + offset;
			triangle->operator[](1) = triangle->operator[](1) + offset;
			triangle->operator[](2) = triangle->operator[](2) + offset;
			
			
		}

	};
}