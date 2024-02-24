#pragma once

#include "Shader.h"

namespace engine::DX
{

	class GeometryShader : public Shader
	{
	public:
		GeometryShader(const std::wstring& pathToShader, const std::string& entryPoint = "main", const std::string& shaderVersion = "gs_5_0")
			: Shader(pathToShader, entryPoint, shaderVersion)
		{
			compileShader();
			createShader();
		}

		void bind() override
		{
			g_devcon->GSSetShader(shader.Get(), nullptr, 0);
		}

		void createShader() override
		{
			HRESULT result = g_device->CreateGeometryShader(shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), nullptr, shader.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Geometry shader wasn't created: ");
			}
		}

	private: 
		ComPtr<ID3D11GeometryShader> shader;
	};
}