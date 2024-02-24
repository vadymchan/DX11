#pragma once
#include "Shader.h"

namespace engine::DX
{
	class DomainShader : public Shader
	{
	public:

		DomainShader(const std::wstring& pathToShader, const std::string& entryPoint = "main", const std::string& shaderVersion = "ds_5_0")
			: Shader(pathToShader, entryPoint, shaderVersion)
		{
			compileShader();
			createShader();
		}

		void bind() override
		{
			g_devcon->DSSetShader(shader.Get(), nullptr, 0);
		}

		void createShader() override
		{
			HRESULT result = g_device->CreateDomainShader(shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), nullptr, shader.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Domain shader wasn't created: ");
			}
		}

	private:
		ComPtr<ID3D11DomainShader> shader;
	};
}
