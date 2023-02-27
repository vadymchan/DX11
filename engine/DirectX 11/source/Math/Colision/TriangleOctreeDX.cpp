#include "TriangleOctreeDX.h"
#include <algorithm>


namespace engine::DX
{
	

	const float TriangleOctree::PREFFERED_TRIANGLE_COUNT = 32;
	const float TriangleOctree::MAX_STRETCHING_RATIO = 1.05f;

	inline const float3& getPos(const Mesh& mesh, size_t triangleIndex, size_t vertexIndex)
	{
		
		return mesh.getVertices().at(mesh.getIndices().at(triangleIndex * 3 + vertexIndex)).position;
		//return (*mesh.getMathMesh().triangles.get())[triangleIndex][vertexIndex];
	}

	inline const float3& getN(const Mesh& mesh, size_t triangleIndex)
	{
		return float3();
		//return (*mesh.getMathMesh().triangles.get())[triangleIndex].getN();
	}

	//entypoint to init
	void TriangleOctree::initialize(Mesh& mesh, Instance* instance)
	{
		m_triangles.clear();
		m_triangles.shrink_to_fit();

		//m_indices.clear();
		//m_indices.shrink_to_fit();

		m_mesh = &mesh;
		m_instance = instance;
		m_children = nullptr;

		const float3 eps = { 1e-5f, 1e-5f, 1e-5f };
		m_box = m_initialBox = { mesh.getBox().min - eps, mesh.getBox().max + eps};
		auto test{ mesh.getIndices().size() / 3.0f };
		for (size_t i = 0; i < (mesh.getIndices().size() / 3.0f); ++i)
		{
			const float3& V1 = getPos(mesh, i, 0);
			const float3& V2 = getPos(mesh, i, 1);
			const float3& V3 = getPos(mesh, i, 2);

			float3 P = (V1 + V2 + V3) / 3.f;
			//m_triangles.emplace_back(i);
			bool inserted = addTriangle(i, V1, V2, V3, P);
		}
		return;
	}
	//for recursive call
	void TriangleOctree::initialize(Mesh& mesh, Instance& instance, const Box& parentBox, const float3& parentCenter, int octetIndex)
	{
		m_mesh = &mesh;
		m_instance = &instance;
		m_children = nullptr;

		const float eps = 1e-5f;
		if (octetIndex % 2 == 0)
		{
			m_initialBox.min.x = parentBox.min.x;
			m_initialBox.max.x = parentCenter.x;
		}
		else
		{
			m_initialBox.min.x = parentCenter.x;
			m_initialBox.max.x = parentBox.max.x;
		}

		if (octetIndex % 4 < 2)
		{
			m_initialBox.min.y = parentBox.min.y;
			m_initialBox.max.y = parentCenter.y;
		}
		else
		{
			m_initialBox.min.y = parentCenter.y;
			m_initialBox.max.y = parentBox.max.y;
		}

		if (octetIndex < 4)
		{
			m_initialBox.min.z = parentBox.min.z;
			m_initialBox.max.z = parentCenter.z;
		}
		else
		{
			m_initialBox.min.z = parentCenter.z;
			m_initialBox.max.z = parentBox.max.z;
		}

		m_box = m_initialBox;
		float3 elongation = (MAX_STRETCHING_RATIO - 1.f) * m_box.size();

		if (octetIndex % 2 == 0) m_box.max.x += elongation.x;
		else m_box.min.x -= elongation.x;

		if (octetIndex % 4 < 2) m_box.max.y += elongation.y;
		else m_box.min.y -= elongation.y;

		if (octetIndex < 4) m_box.max.z += elongation.z;
		else m_box.min.z -= elongation.z;
	}

	bool TriangleOctree::addTriangle(size_t triangleIndex, const float3& V1, const float3& V2, const float3& V3, const float3& center)
	{
		if (!m_initialBox.contains(center) ||
			!m_box.contains(V1) ||
			!m_box.contains(V2) ||
			!m_box.contains(V3))
		{
			return false;
		}

		if (m_children == nullptr)
		{

			//if (m_mesh->getIndices().size() / 3.0f < PREFFERED_TRIANGLE_COUNT) // only one box
			//{
			//	return true;
			//}
			if (m_triangles.size() < PREFFERED_TRIANGLE_COUNT) // no need to init children nodes
			{
				m_triangles.emplace_back(triangleIndex); 
				return true;
			}
			else
			{
				float3 C = (m_initialBox.min + m_initialBox.max) / 2.f; //center

				m_children.reset(new std::array<TriangleOctree, 8>());
				for (int i = 0; i < 8; ++i)
				{
					(*m_children)[i].initialize(*m_mesh, *m_instance,  m_initialBox, C, i);
				}

				std::vector<size_t> newTriangles;

				for (size_t index : m_triangles)
				{
					const float3& P1 = getPos(*m_mesh, index, 0);
					const float3& P2 = getPos(*m_mesh, index, 1);
					const float3& P3 = getPos(*m_mesh, index, 2);

					float3 P = (P1 + P2 + P3) / 3.f;

					int i = 0;
					for (; i < 8; ++i)
					{
						if ((*m_children)[i].addTriangle(index, P1, P2, P3, P))
							break;
					}

					if (i == 8)
						newTriangles.emplace_back(index);
				}

				m_triangles = std::move(newTriangles);
			}
		}

		int i = 0;
		for (; i < 8; ++i)
		{
			if ((*m_children)[i].addTriangle(triangleIndex, V1, V2, V3, center))
				break;
		}

		if (i == 8)
			m_triangles.emplace_back(triangleIndex);

		return true;
	}

	bool TriangleOctree::intersect(ray ray, Intersection& nearest) const
	{

		//check for nullptr

		float4x4 test1{ (m_mesh->getMeshToModelMat(0).Transpose() * m_instance->toWorldMatrix.Transpose())}; // test it
		auto test{ test1.Invert()};
		ray.position = float3::Transform(ray.position, test);

		float boxT = nearest.t;
		if (!m_box.hit(ray, boxT))
		{
			return false;
		}

		nearest.point = float3::Transform(ray.position, (m_mesh->getMeshToModelMat(0).Transpose() * m_instance->toWorldMatrix.Transpose())) + ray.direction * boxT;
		nearest.t = boxT;
		return true;

		return intersectInternal(ray, nearest);
	}

	bool TriangleOctree::intersectInternal(const ray& ray, Intersection& nearest) const
	{
		{
			float boxT = nearest.t;
			if (!m_box.hit(ray, boxT))//ray-box intersection
				return false;
		}


		bool found = false;

		for (size_t i = 0; i < m_triangles.size(); ++i)
		{
			const float3& V1 = getPos(*m_mesh, m_triangles[i], 0);
			const float3& V2 = getPos(*m_mesh, m_triangles[i], 1);
			const float3& V3 = getPos(*m_mesh, m_triangles[i], 2);

			
			//if (hit(ray, nearest.t, V1, V2, V3)) // ray-triangle intersection in current box
			float t = nearest.t;
			if (ray.Intersects(V1, V2, V3, t)) // ray-triangle intersection in current box
			{
				if (t < nearest.t)
				{

					nearest.point = float3::Transform(ray.position, (m_mesh->getMeshToModelMat(0).Transpose() * m_instance->toWorldMatrix.Transpose())) +  ray.direction * t;
					nearest.t = t;
					found = true;
				}
				
			}
		}

		if (!m_children) return found; //no child nodes - return 

		struct OctantIntersection
		{
			int index;
			float t;
		};

		std::array<OctantIntersection, 8> boxIntersections;

		for (int i = 0; i < 8; ++i)
		{
			if ((*m_children)[i].m_box.contains(ray.position))
			{
				boxIntersections[i].index = i;
				boxIntersections[i].t = 0.f;
			}
			else
			{
				float boxT = nearest.t;
				if ((*m_children)[i].m_box.hit(ray, nearest.t))
				{
					boxIntersections[i].index = i;
					boxIntersections[i].t = boxT;
				}
				else
				{
					boxIntersections[i].index = -1;
				}
			}
		}

		std::sort(boxIntersections.begin(), boxIntersections.end(),
			[](const OctantIntersection& A, const OctantIntersection& B) -> bool { return A.t < B.t; });

		for (int i = 0; i < 8; ++i)
		{
			if (boxIntersections[i].index < 0 || boxIntersections[i].t > nearest.t)
				continue;

			if ((*m_children)[boxIntersections[i].index].intersectInternal(ray, nearest))
			{
				found = true;
			}
		}

		return found;
	}

}