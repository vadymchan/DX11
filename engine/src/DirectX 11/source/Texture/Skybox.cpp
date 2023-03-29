#include "Skybox.h"

namespace engine::DX
{
	const std::array<Skybox::Vertex, 3> Skybox::m_vertices = {

		Vertex{{-1, -1, 0}, {0, 1}},
		Vertex{{-1, 1,  0}, {0, 0}},
		Vertex{{1, -1,  0}, {1, 1}},

	};

	const std::vector<UINT> Skybox::Vertex::alignedByteOffsets =
	{
		0,
		sizeof(Skybox::Vertex::position)
	};

	VertexBuffer<Skybox::Vertex> Skybox::m_vertexBuffer;
}
