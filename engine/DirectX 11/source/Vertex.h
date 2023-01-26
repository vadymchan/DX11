#pragma once

#include "../render/D3D.h"
#include <array>

namespace DX::engine
{
	struct Vertex
	{
		FLOAT x{};
		FLOAT y{};
		FLOAT z{};
		DirectX::XMFLOAT4 colorRGBA;

		Vertex(FLOAT x, FLOAT y, FLOAT z, const DirectX::XMFLOAT4& colorRGBA)
			: x{ x }
			, y{ y }
			, z{ z }
			, colorRGBA{ colorRGBA }
		{}

	};
}

