#pragma once

#include "D3D/D3D.h"

namespace engine::DX
{
	struct VertexType
	{
		DirectX::XMFLOAT4 position{};
		DirectX::XMFLOAT4 colorRGBA{};

		VertexType(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& colorRGBA)
			: position {position}
			, colorRGBA{ colorRGBA }
		{}

	};
}

