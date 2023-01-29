#pragma once
#include "../../dependencies/glm-0.9.9.9/glm/glm.hpp"
#include <vector>
#include <memory>
#include "triangle.h"
#include "box.h"


namespace Engine
{
	class Cube
	{
	public:

		glm::mat4 tMat{};
		Cube(const glm::mat4& tMat)
			:  tMat(tMat)
		{}

		bool hit(ray r, Intersection& near, glm::vec3& intersectionN);

		static const std::shared_ptr<std::vector<Triangle>> triangles;
		static std::shared_ptr<Box> box;


	};

	
	

}