#pragma once
#include "../render/D3D.h"

namespace DX::engine
{
	struct ConstBuffer
	{
		DirectX::XMFLOAT2 viewport{};
		DirectX::XMFLOAT2 time{};

	};

}