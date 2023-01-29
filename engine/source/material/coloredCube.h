#pragma once
#include "../math/cube.h"
#include "material.h"

namespace Engine
{
	class ColorCube
	{
	public:
		Material material;

		ColorCube(const glm::mat4& tMat, const Material& material)
			: mathMesh(Cube(tMat))
			, material(material)
		{}

		bool hit(const ray& r, Intersection& near_t, glm::vec3& intersectionN) { return mathMesh.hit(r, near_t, intersectionN); }

		 Cube* getPtrMathMesh() { return &mathMesh; }
		 const Cube& getMathMesh() const { return mathMesh; }

	private:
		Cube mathMesh;

	};


}