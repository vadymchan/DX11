#pragma once
#include "Shader.h"

namespace engine::DX
{
	class PixelShader : public Shader
	{
	public:
		PixelShader(const std::wstring& pathToShader, const std::string& entryPoint = "main", const std::string& shaderVersion = "ps_4_0")
			: Shader(pathToShader, entryPoint, shaderVersion)
		{
			compileShader();
			createShader();
		}

		void bind() override
		{
			g_devcon->PSSetShader(shader.Get(), 0, 0);
		}

		void createShader() override
		{
			HRESULT result = g_device->CreatePixelShader(shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), nullptr, shader.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Pixel shader wasn't created: ");
			}
		}

	private:
		ComPtr<ID3D11PixelShader> shader;

	};
}

