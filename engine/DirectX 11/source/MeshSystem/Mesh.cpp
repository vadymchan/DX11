#include "Mesh.h"

namespace engine::DX
{
	const std::vector<UINT> Mesh::Vertex::alignedByteOffsets =
	{
		0,
		sizeof(Mesh::Vertex::position)
	};
}

