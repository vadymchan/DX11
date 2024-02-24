#pragma once
#include "Shader.h"

namespace engine::DX
{
	class HullShader : public Shader
	{
	public:
		HullShader(const std::wstring& pathToShader, const std::string& entryPoint = "main", const std::string& shaderVersion = "hs_5_0")
			: Shader(pathToShader, entryPoint, shaderVersion)
		{
			compileShader();
			createShader();
		}

		void bind() override
		{
			g_devcon->HSSetShader(shader.Get(), nullptr, 0);
		}

		void createShader() override
		{
			HRESULT result = g_device->CreateHullShader(shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), nullptr, shader.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Hull shader wasn't created: ");
			}
		}

	private:
		ComPtr<ID3D11HullShader> shader;
	};
}