#include "Mesh.h"

namespace engine::DX
{
	const std::vector<UINT> Mesh::Vertex::alignedByteOffsets =
	{
		0,
		sizeof(Mesh::Vertex::position),
		sizeof(Mesh::Vertex::position) + sizeof(Mesh::Vertex::normal),
		sizeof(Mesh::Vertex::position) + sizeof(Mesh::Vertex::normal) + sizeof(Mesh::Vertex::texCoord),
		sizeof(Mesh::Vertex::position) + sizeof(Mesh::Vertex::normal) + sizeof(Mesh::Vertex::texCoord) + sizeof(Mesh::Vertex::tangent),
	};
}

