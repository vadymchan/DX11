#include "ShaderManager.h"

namespace engine::DX
{

//vertex shader
//---------------------------------------------------------------------------------------------------------------------------------------------------

void engine::DX::ShaderManager::addVertexShader(const std::wstring& fileName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc, const std::string& entrancePoint, const std::string& shaderVersion)
{
	assert(vertexShaders.find(fileName) == vertexShaders.end() && "vertex shader is already created");
	VertexShader vertexShader = VertexShader(shaderDirectory + fileName, entrancePoint, shaderVersion);
	vertexShader.setInputElementDesc(inputElementDesc);
	vertexShaders[fileName] = std::make_shared<VertexShader>(std::move(vertexShader));
}

bool engine::DX::ShaderManager::getVertexShader(const std::wstring& fileName, std::shared_ptr<VertexShader>& vertexShader)
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

std::shared_ptr<VertexShader> engine::DX::ShaderManager::getVertexShader(const std::wstring& fileName)
{
	auto value = vertexShaders.find(fileName);
	if (value == vertexShaders.end())
	{
		std::cerr << "Vertex shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
		return nullptr;
	}
	return value->second;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------


//pixel shader
//---------------------------------------------------------------------------------------------------------------------------------------------------

void engine::DX::ShaderManager::addPixelShader(const std::wstring& fileName, const std::string& entrancePoint, const std::string& shaderVersion)
{
	assert(vertexShaders.find(fileName) == vertexShaders.end() && "Pixel shader is already created");
	pixelShaders[fileName] = std::make_shared<PixelShader>(std::move(PixelShader(shaderDirectory + fileName, entrancePoint, shaderVersion)));
}


bool engine::DX::ShaderManager::getPixelShader(const std::wstring& fileName, std::shared_ptr<PixelShader>& pixelShader)
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

std::shared_ptr<PixelShader> engine::DX::ShaderManager::getPixelShader(const std::wstring& fileName)
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



}