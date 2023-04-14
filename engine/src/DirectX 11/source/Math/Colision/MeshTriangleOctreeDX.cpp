#include "MeshTriangleOctreeDX.h"
#include <algorithm>


namespace engine::DX
{


	const float MeshTriangleOctree::PREFFERED_TRIANGLE_COUNT = 50;
	const float MeshTriangleOctree::MAX_STRETCHING_RATIO = 1.01f;

	const float3& MeshTriangleOctree::getPos(const Mesh& mesh, size_t triangleIndex, size_t vertexIndex) const
	{
		return mesh.getVertices().at(mesh.getIndices().at(triangleIndex * 3 + vertexIndex)).position;
	}

	

	void MeshTriangleOctree::countTriangles(MeshTriangleOctree* tri, int& count, std::vector<int>& triangles)
	{
		if (tri == nullptr)
		{
			return;
		}
		else
		{
			count += tri->m_triangles.size();
			for (auto& i : tri->m_triangles)
			{
				triangles.push_back(i);
			}
		}
		if (tri->m_children == nullptr)
		{
			return;
		}
		countTriangles(&tri->m_children->at(0), count, triangles);
		countTriangles(&tri->m_children->at(1), count, triangles);
		countTriangles(&tri->m_children->at(2), count, triangles);
		countTriangles(&tri->m_children->at(3), count, triangles);
		countTriangles(&tri->m_children->at(4), count, triangles);
		countTriangles(&tri->m_children->at(5), count, triangles);
		countTriangles(&tri->m_children->at(6), count, triangles);
		countTriangles(&tri->m_children->at(7), count, triangles);
		return;
	}

	//entypoint to init
	void MeshTriangleOctree::initialize(Mesh& mesh)
	{
		m_triangles.clear();
		m_triangles.shrink_to_fit();
		m_mesh = &mesh;
		//m_instance = instance;
		m_children = nullptr;

		const float3 eps = { 1e-10f, 1e-10f, 1e-10f };
		m_box = m_initialBox = { mesh.getBox().min - eps, mesh.getBox().max + eps };
		auto triangleNum{ mesh.getIndices().size() / 3.0f };
		float remainingTriangles{ triangleNum };
		for (size_t i = 0; i < triangleNum; ++i)
		{

			if (triangleNum < PREFFERED_TRIANGLE_COUNT) // only one box
			{
				m_triangles.emplace_back(i);
				continue;
			}

			const float3& V1 = getPos(mesh, i, 0);
			const float3& V2 = getPos(mesh, i, 1);
			const float3& V3 = getPos(mesh, i, 2);


			float3 P = (V1 + V2 + V3) / 3.f;

			bool inserted = addTriangle(i, V1, V2, V3, P);
		}

		return;
	}



	bool MeshTriangleOctree::addTriangle(size_t triangleIndex, const float3& V1, const float3& V2, const float3& V3, const float3& center)
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
			m_triangles.emplace_back(triangleIndex);
			if (m_triangles.size() < PREFFERED_TRIANGLE_COUNT) // no need to init children nodes
			{

				return true;
			}
			else
			{
				float3 C = (m_initialBox.min + m_initialBox.max) / 2.f; //center

				m_children.reset(new std::array<MeshTriangleOctree, 8>());
				for (int i = 0; i < 8; ++i)
				{
					(*m_children)[i].initialize(*m_mesh, m_initialBox, C, i);
				}

				//move triangles to children
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

					if (i == 8) // between two child boxes
					{
						newTriangles.emplace_back(index);
					}
				}
				//m_triangles.clear();
				m_triangles = std::move(newTriangles);

			}
		}
		else
		{
			int i = 0;
			for (; i < 8; ++i)
			{
				if ((*m_children)[i].addTriangle(triangleIndex, V1, V2, V3, center))
					break;
			}

			if (i == 8) // between two child boxes
			{
				m_triangles.emplace_back(triangleIndex);
			}

			return true;
		}
		return false;
	}

	//for recursive call
	void MeshTriangleOctree::initialize(Mesh& mesh, const Box& parentBox, const float3& parentCenter, int octetIndex)
	{
		m_mesh = &mesh;
		m_children = nullptr;

		const float eps = 1e-10f;

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

	static ray oldray;


	bool MeshTriangleOctree::intersect(ray r, Intersection& nearest, Instance& instance) const
	{

		oldray = r;

		float4x4 meshToWorld{ (TransformSystem::getInstance().getTransform(instance.worldMatrixID) * m_mesh->getMeshToModelMat(0)).Transpose()};
		auto rayToMesh{ meshToWorld.Invert() };

		r.position = float3::Transform(r.position, rayToMesh);
		r.direction = (float3::TransformNormal(r.direction, rayToMesh));
		r.direction.Normalize();

		float boxT = nearest.t;
		if (!m_box.hit(r, boxT))
		{
			return false;
		}
		

		return intersectInternal(r, nearest);
	}

	bool MeshTriangleOctree::intersectInternal(const ray& ray, Intersection& nearest) const
	{
		{
			float boxt = nearest.t;
			if (!m_box.hit(ray, boxt))//ray-box intersection
				return false;
		}


		bool found = false;

		for (size_t i = 0; i < m_triangles.size(); ++i)
		{
			const float3& V1 = getPos(*m_mesh, m_triangles[i], 0);
			const float3& V2 = getPos(*m_mesh, m_triangles[i], 1);
			const float3& V3 = getPos(*m_mesh, m_triangles[i], 2);

			float t = nearest.t;
			if (ray.Intersects(V1, V2, V3, t)) // ray-triangle intersection in current box
			{
				if (t < nearest.t)
				{
					nearest.point = oldray.position + oldray.direction * t;
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

			float boxT = nearest.t;
			if ((*m_children)[i].m_box.hit(ray, boxT))
			{
				boxIntersections[i].index = i;
				boxIntersections[i].t = boxT;
			}
			else
			{
				boxIntersections[i].index = -1;
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
