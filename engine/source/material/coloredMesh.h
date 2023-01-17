#pragma once
#include "../math/mesh.h"
#include "material.h"

namespace Engine
{
	class ColorMesh
	{
	public:
		Material material;


		ColorMesh(const std::shared_ptr<std::vector<Triangle>>& triangles, const glm::mat4& tMat, const std::shared_ptr<Box>& box, const Material& material)
			: mathMesh(Mesh (triangles, tMat, box))
			, material(material)
		{}


		bool hit(const ray& r, float& near_t, glm::vec3& intersectionN) { return mathMesh.hit(r, near_t, intersectionN); }

		 Mesh* getPtrMathMesh() { return &mathMesh; }
		 const Mesh& getMathMesh() const { return mathMesh; }

	private:
		Mesh mathMesh;

	};


}