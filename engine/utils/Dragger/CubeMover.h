#pragma once
#include "IObjectMover.h"
#include "../../source/math/cube.h"


namespace Engine
{
	class CubeMover : public IObjectMover
	{
	public:
		Cube* cube;


		CubeMover(Cube* mesh,float cameraToNearPlane, const glm::vec3& intersectionPoint)
		{
			this->cube = mesh;
			this->cameraToNearPlane = cameraToNearPlane;
			this->intersectionPoint = intersectionPoint;
		}
		
		virtual void move(const glm::vec3& offset) override
		{

			cube->tMat[3][0] += offset.x;
			cube->tMat[3][1] += offset.y;
			cube->tMat[3][2] += offset.z;



		}

	};
}