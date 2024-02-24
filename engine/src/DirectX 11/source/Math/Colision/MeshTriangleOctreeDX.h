#pragma once

#include "boxDX.h"
#include "../../MeshSystem/Mesh.h"
#include "../../MeshSystem/Instance/OpaqueInstances.h"
#include "RayIntersection.h"

#include <memory>
#include <vector>
#include <array>


namespace engine::DX
{


	class MeshTriangleOctree
	{
	public:

		

		const static float PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		void clear() { m_mesh = nullptr; }
		bool inited() const { return m_mesh != nullptr; }

		void initialize(Mesh& mesh);

		bool intersect(ray r, RayIntersection& nearest, TransformSystem::ID& instance) const;

		Mesh* m_mesh = nullptr;
	protected:
		std::vector<size_t> m_triangles;
		std::unique_ptr<std::array<MeshTriangleOctree, 8>> m_children;
		Box m_box;
		Box m_initialBox;

#pragma region utility functions
		void countTriangles(MeshTriangleOctree* tri, int& count, std::vector<int>& triangles);
		//ray-triangle interseciton
		bool intersects(const float3& Origin, const float3& Direction, const float3& V0, const float3& V1, const float3& V2, float& Dist) const noexcept;
#pragma endregion


		const float3& getPos(const Mesh& mesh, size_t triangleIndex, size_t vertexIndex) const;
		void initialize(Mesh& mesh, const Box& parentBox, const float3& parentCenter, int octetIndex);
		bool addTriangle(size_t triangleIndex, const float3& V1, const float3& V2, const float3& V3, const float3& center);
		bool intersectInternal(const ray& ray, RayIntersection& nearest) const;
	};
}
