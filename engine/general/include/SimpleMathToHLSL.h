#pragma once
#include <SimpleMath.h>
/*
*
* this header is used in DirectX to shorten 
* Simple Math types such as DirectX::SimpleMath::Quaternion.
* 
*/

namespace engine::DX
{
	//type aliases as in HLSL
	using float2 = DirectX::SimpleMath::Vector2;
	using float3 = DirectX::SimpleMath::Vector3;
	using float4 = DirectX::SimpleMath::Vector4;
	using float4x4 = DirectX::SimpleMath::Matrix;

	using quat = DirectX::SimpleMath::Quaternion;
	using ray = DirectX::SimpleMath::Ray;
}