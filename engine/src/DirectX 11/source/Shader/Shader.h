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

		void compileShader()
		{
			

			HRESULT result = D3DCompileFromFile((pathToShader).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), shaderVersion.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &shaderBinary, errorMessage.ReleaseAndGetAddressOf());
			
			
			if (FAILED(result))
			{
				if (errorMessage.Get() != nullptr)
				{
					char* pErrorMessage = (char*)errorMessage->GetBufferPointer();
					std::cerr << "Shader compilation failed with error message: "
						<< std::string(pErrorMessage, pErrorMessage + errorMessage->GetBufferSize()) << std::endl;
				}
				else
				{
					std::cerr << "Shader compilation failed with unknown error" << std::endl;
				}
			}
		};
		virtual void bind() = 0;
		virtual void createShader() = 0;

	protected:
		ComPtr<ID3DBlob> shaderBinary; 
		ComPtr<ID3D10Blob> errorMessage;
		std::wstring pathToShader;
		std::string entryPoint;
		std::string shaderVersion;

	};
}

