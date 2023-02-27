#pragma once

#pragma once

#include "boxDX.h"
#include "../../MeshSystem/Mesh.h"
#include "../../MeshSystem/OpaqueInstances.h"

#include <memory>
#include <vector>
#include <array>



namespace engine::DX
{
	using Instance = OpaqueInstances::Instance;

	class TriangleOctree
	{
	public:

		struct Intersection
		{
			float3 point;
			float3 normal;
			float3 dir;
			float t;

			bool isValid() { return std::isfinite(t); }

			void reset() { t = std::numeric_limits<float>::infinity(); }
		};

		const static float PREFFERED_TRIANGLE_COUNT;
		const static float MAX_STRETCHING_RATIO;

		void clear() { m_mesh = nullptr; }
		bool inited() const { return m_mesh != nullptr; }

		void initialize(Mesh& mesh, Instance* instance);

		bool intersect( ray, Intersection& nearest) const;

		Mesh* m_mesh = nullptr;
		Instance* m_instance = nullptr;
	protected:
		std::vector<size_t> m_triangles;
		//std::vector<uint32_t> m_indices;


		Box m_box;
		Box m_initialBox;

		std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

		void initialize(Mesh& mesh, Instance& instance, const Box& parentBox, const float3& parentCenter, int octetIndex);

		bool addTriangle(size_t triangleIndex, const float3& V1, const float3& V2, const float3& V3, const float3& center);


		bool intersectInternal(const ray& ray, Intersection& nearest) const;
	};
}
