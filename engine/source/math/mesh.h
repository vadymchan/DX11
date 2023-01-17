#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/glm.hpp"
#include <vector>
#include <memory>
#include "triangle.h"
#include "box.h"

namespace Engine
{
	class Mesh
	{
	public:
		glm::mat4 tMat{};
		Mesh(std::shared_ptr<std::vector<Triangle>> triangles, const glm::mat4& tMat, const std::shared_ptr<Box>& meshBox)
			: triangles (triangles)
			, tMat(tMat)
			, box(meshBox)
		{}

		bool hit(ray r, float& near, glm::vec3& intersectionN);

		//const std::shared_ptr<std::vector<Triangle>>& GetTriangles() const { return triangles; }

		std::shared_ptr<std::vector<Triangle>> triangles;
		std::shared_ptr<Box> box;


	};


}