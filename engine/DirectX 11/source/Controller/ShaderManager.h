#pragma once
#include <unordered_map>
#include <string>
#include "../Shader/VertexShader.h"
#include "../Shader/PixelShader.h"

namespace engine::DX
{
	const std::wstring shaderDirectory = L"engine\\general\\resources\\shaders\\";

	class ShaderManager
	{
	public:

		static ShaderManager& getInstance()
		{
			static ShaderManager instance;
			return instance;
		}

		//vertex shader
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		void addVertexShader(const std::wstring& fileName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc, const std::string& entrancePoint = "main", const std::string& shaderVersion = "vs_4_0")
		{
			VertexShader vertexShader = VertexShader(shaderDirectory + fileName , entrancePoint, shaderVersion);
			vertexShader.setInputElementDesc(inputElementDesc);
			vertexShaders[fileName] = std::make_shared<VertexShader>(std::move(vertexShader));
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		bool getVertexShader(const std::wstring& fileName, std::shared_ptr<VertexShader>& vertexShader)
		{
			auto value = vertexShaders.find(fileName);
			if (value == vertexShaders.end())
			{
				std::cerr << "Vertex shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
				return false;
			}
			vertexShader = value->second;
			return true;
		}
		//analog
		std::shared_ptr<VertexShader> getVertexShader(const std::wstring& fileName)
		{
			auto value = vertexShaders.find(fileName);
			if (value == vertexShaders.end())
			{
				std::cerr << "Vertex shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
				return nullptr;
			}
			return value->second;
		}

		//set inputlayout desc to 
		//---------------------------------------------------------------------------------------------------------------------------------------------------

		//pixel shader
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		void addPixelShader(const std::wstring& fileName, const std::string& entrancePoint = "main", const std::string& shaderVersion = "ps_4_0")
		{
			pixelShaders[fileName] = std::make_shared<PixelShader>(std::move(PixelShader(shaderDirectory + fileName, entrancePoint, shaderVersion)));
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		bool getPixelShader(const std::wstring& fileName, std::shared_ptr<PixelShader>& pixelShader)
		{
			auto value = pixelShaders.find(fileName);
			if (value == pixelShaders.end())
			{
				std::cerr << "Pixel shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
				return false;
			}
			pixelShader = value->second;
			return true;
		}
		//analog 
		std::shared_ptr<PixelShader> getPixelShader(const std::wstring& fileName)
		{
			auto value = pixelShaders.find(fileName);
			if (value == pixelShaders.end())
			{
				std::cerr << "Pixel shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
				return nullptr;
			}
			
			return value->second;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------------------

	private:
		ShaderManager() = default;
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;
		std::unordered_map<std::wstring, std::shared_ptr<VertexShader>> vertexShaders{};
		std::unordered_map<std::wstring, std::shared_ptr<PixelShader>> pixelShaders{};

	};
}
