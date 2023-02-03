#pragma once

#include "../render/D3D.h"

namespace engine::DX
{
	struct VertexType
	{
		FLOAT x{};
		FLOAT y{};
		FLOAT z{};
		DirectX::XMFLOAT4 colorRGBA{};

		VertexType(FLOAT x, FLOAT y, FLOAT z, const DirectX::XMFLOAT4& colorRGBA)
			: x{ x }
			, y{ y }
			, z{ z }
			, colorRGBA{ colorRGBA }
		{}

	};
}

