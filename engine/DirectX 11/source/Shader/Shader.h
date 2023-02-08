#pragma once
#include "../D3D/D3D.h"

namespace engine::DX
{
	class Shader
	{
	public:

		Shader(const std::wstring& pathToShader, const std::string& entryPoint, const std::string& shaderVersion)
			: pathToShader{ pathToShader }
			, shaderVersion{ shaderVersion }
			, entryPoint{ entryPoint }
		{

		}

		virtual void compileShader() = 0;
		virtual void bind() = 0;
		virtual void createShader() = 0;

	protected:
		ComPtr<ID3DBlob> shaderBinary;
		std::wstring pathToShader;
		std::string entryPoint;
		std::string shaderVersion;

	};
}

