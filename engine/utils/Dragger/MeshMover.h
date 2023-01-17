#pragma once
#include "IObjectMover.h"
#include "../../source/math/mesh.h"


namespace Engine
{
	class MeshMover : public IObjectMover
	{
	public:
		Mesh* mesh;


		MeshMover(Mesh* mesh, float aspectRatio)
		{
			this->mesh = mesh;
			rayNearPlaneRatio = aspectRatio;
		}
		
		virtual void move(const glm::vec3& offset) override
		{

			mesh->tMat[3][0] += offset.x;
			mesh->tMat[3][1] += offset.y;
			mesh->tMat[3][2] += offset.z;



		}

	};
}