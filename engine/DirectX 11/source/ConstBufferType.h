#pragma once
#include "D3D/D3D.h"

namespace engine::DX
{
	struct ConstBufferType
	{
		DirectX::XMFLOAT2 viewport{};
		DirectX::XMFLOAT2 time{};

	};

}