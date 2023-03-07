#pragma once

#include "boxDX.h"
#include "../../MeshSystem/Mesh.h"
#include "../../MeshSystem/OpaqueInstances.h"

#include <memory>
#include <vector>
#include <array>



//namespace engine::DX
//{
//	using Instance = OpaqueInstances::Instance;
//
//	class MeshTriangleOctree
//	{
//	public:
//
//		struct Intersection
//		{
//			float3 point;
//			float3 normal;
//			float3 dir;
//			float t;
//
//			bool isValid() { return std::isfinite(t); }
//
//			void reset() { t = std::numeric_limits<float>::infinity(); }
//		};
//
//		const static float PREFFERED_TRIANGLE_COUNT;
//		const static float MAX_STRETCHING_RATIO;
//
//		void clear() { m_model = nullptr; }
//		bool inited() const { return m_model != nullptr; }
//
//		MeshTriangleOctree(Model& model, Instance* instance)
//		{
//			initialize(model, instance);
//		}
//		MeshTriangleOctree() = default;
//
//		void initialize(Model& model, Instance* instance);
//
//		bool intersect( ray, Intersection& nearest) const;
//
//	protected:
//		Model* m_model = nullptr;
//		std::weak_ptr<Instance> m_instance;
//		std::vector<std::pair<size_t, size_t>> m_meshTriangles; //first - mesh index, second - triangle index
//		std::unique_ptr<std::array<MeshTriangleOctree, 8>> m_children;
//		Box m_box = Box::empty();
//		Box m_initialBox = Box::empty();
//
//		void countTriangles(MeshTriangleOctree* tri, int& count, std::vector<std::pair<size_t, size_t>>& triangles);
//		const float3& getPos(int meshIndex, size_t triangleIndex, size_t vertexIndex) const;
//		void initialize(Model& model, Instance& instance, const Box& parentBox, const float3& parentCenter, int octetIndex);
//		bool addTriangle(size_t meshIndex, size_t triangleIndex, const float3& V1, const float3& V2, const float3& V3, const float3& center);
//		bool intersectInternal(const ray& ray, Intersection& nearest) const;
//	};
//}



namespace engine::DX
{
	using Instance = OpaqueInstances::Instance;

	class MeshTriangleOctree
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

		void initialize(Mesh& mesh, std::weak_ptr<Instance> instance);

		bool intersect(ray r, Intersection& nearest) const;

		Mesh* m_mesh = nullptr;
		std::weak_ptr<Instance> m_instance;
	protected:
		std::vector<size_t> m_triangles;
		std::unique_ptr<std::array<MeshTriangleOctree, 8>> m_children;
		Box m_box;
		Box m_initialBox;

		void countTriangles(MeshTriangleOctree* tri, int& count, std::vector<int>& triangles);
		const float3& getPos(const Mesh& mesh, size_t triangleIndex, size_t vertexIndex) const;
		void initialize(Mesh& mesh, const std::weak_ptr<Instance>& instance, const Box& parentBox, const float3& parentCenter, int octetIndex);
		bool addTriangle(size_t triangleIndex, const float3& V1, const float3& V2, const float3& V3, const float3& center);
		bool intersectInternal(const ray& ray, Intersection& nearest) const;
	};
}
