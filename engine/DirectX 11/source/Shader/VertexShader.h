#pragma once


#include "Shader.h"

namespace engine::DX
{
	class VertexShader : public Shader
	{
	public:

		VertexShader(const std::wstring& pathToShader, const std::string& entryPoint = "main", const std::string& shaderVersion = "vs_4_0")
			: Shader(pathToShader, entryPoint, shaderVersion)
		{
			compileShader();
			createShader();
		}

		void compileShader() override
		{
			HRESULT result = D3DCompileFromFile((pathToShader).c_str(), 0, 0, entryPoint.c_str(), shaderVersion.c_str(), 0, 0, &shaderBinary, 0);
			if (FAILED(result))
			{
				std::cerr << "Vertex shader was not initialized\n";
			}
		}

		void bind()
		{
			if (shaderBinary == nullptr)
			{
				compileShader();
			}
			if (shader.Get() == nullptr)
			{
				createShader();
			}
			createInputLayout();
			setInputLayout();
			g_devcon->VSSetShader(shader.Get(), 0, 0);

		}

		void setInputElementDesc(const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputlementDescriptions)
		{
			inputElementDesc = inputlementDescriptions;
			updateInputLayout = true;
		}

		void addInputElement(const LPCSTR& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT inputSlot, UINT alignedByteOffset, D3D11_INPUT_CLASSIFICATION inputSlotClass, UINT instanceDataStepRate)
		{
			D3D11_INPUT_ELEMENT_DESC inputElementDesc;
			inputElementDesc.SemanticName = semanticName;
			inputElementDesc.SemanticIndex = semanticIndex; // vertex shader
			inputElementDesc.Format = format;
			inputElementDesc.InputSlot = inputSlot; //vertex buffer slot
			inputElementDesc.AlignedByteOffset = alignedByteOffset;
			inputElementDesc.InputSlotClass = inputSlotClass;
			inputElementDesc.InstanceDataStepRate = instanceDataStepRate;

			this->inputElementDesc.emplace_back(std::move(inputElementDesc));
			updateInputLayout = true;
		}

		void setInputLayout()
		{
			g_devcon->IASetInputLayout(inputLayout.Get());
		}

		void createInputLayout()
		{
			if (updateInputLayout)
			{
				g_device->CreateInputLayout(inputElementDesc.data(), inputElementDesc.size(), shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), inputLayout.GetAddressOf());
				updateInputLayout = false;
			}
		}

		void createShader()
		{
			g_device->CreateVertexShader(shaderBinary->GetBufferPointer(), shaderBinary->GetBufferSize(), nullptr, shader.GetAddressOf());
		}

	private:
		ComPtr<ID3D11VertexShader> shader;
		ComPtr<ID3D11InputLayout> inputLayout;
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;
		bool updateInputLayout{false};
	};
}
