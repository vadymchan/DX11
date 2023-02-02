#pragma once
#include "../render/D3D.h"

namespace engine::DX
{
	struct ConstBuffer
	{
		DirectX::XMFLOAT2 viewport{};
		DirectX::XMFLOAT2 time{};

	};

}