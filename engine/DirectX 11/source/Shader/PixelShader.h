#pragma once
#include "Shader.h"

namespace engine::DX
{
	class PixelShader : public Shader
	{
	public:
		PixelShader(const std::wstring& pathToShader, const std::string& entryPoint = "main", const std::string& shaderVersion = "vs_4_0")
			: Shader(pathToShader, entryPoint, shaderVersion)
		{
			compileShader();
			createShader();
		}

		void bind() override
		{
			if (shaderBinary == nullptr)
			{
				compileShader();
			}
			if (shader.Get() == nullptr)
			{
				createShader();
			}

			g_devcon->PSSetShader(shader.Get(), 0, 0);
		}

		void createShader() override
		{
			g_device->CreatePixelShader(shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), nullptr, shader.GetAddressOf());
		}

	private:
		ComPtr<ID3D11PixelShader> shader;

	};
}

