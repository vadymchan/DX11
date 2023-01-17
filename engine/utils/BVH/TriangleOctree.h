#pragma once

#pragma once

#include "../../source/math/box.h"
#include "../../source/math/mesh.h"
#include "../../source/math/ray.h"
#include "../../dependencies/glm-0.9.9.9/glm/ext/vector_float3.hpp"
#include <memory>
#include <vector>
#include "../../source/material/coloredMesh.h"
#include "../../source/math/intersection.h"

namespace Engine
{




	class TriangleOctree 
	{
	public:
		const static int PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		void clear() { m_mesh = nullptr; }
		bool inited() const { return m_mesh != nullptr; }

		void initialize(ColorMesh& mesh);

		bool intersect(ray ray, Intersection& nearest) const;

		ColorMesh* m_mesh = nullptr;
	protected:
		std::vector<uint32_t> m_triangles;


		Box m_box;
		Box m_initialBox;

		std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

		void initialize(ColorMesh& mesh, const Box& parentBox, const glm::vec3& parentCenter, int octetIndex);

		bool addTriangle(uint32_t triangleIndex, const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3, const glm::vec3& center);


		bool intersectInternal(const ray& ray, Intersection& nearest) const;
	};
}
