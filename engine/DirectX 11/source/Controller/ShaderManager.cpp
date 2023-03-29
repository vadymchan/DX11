#include "ShaderManager.h"

namespace engine::DX
{

	//vertex shader
	//---------------------------------------------------------------------------------------------------------------------------------------------------

	void engine::DX::ShaderManager::addVertexShader(const std::wstring& fileName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc, const std::string& entrancePoint, const std::string& shaderVersion)
	{
		assert(vertexShaders.find(fileName) == vertexShaders.end() && "Vertex shader is already created");
		VertexShader vertexShader = VertexShader(shaderDirectory + fileName, entrancePoint, shaderVersion);
		vertexShader.setInputElementDesc(inputElementDesc);
		vertexShaders[fileName] = std::make_shared<VertexShader>(std::move(vertexShader));
	}


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
		assert(pixelShaders.find(fileName) == pixelShaders.end() && "Pixel shader is already created");
		pixelShaders[fileName] = std::make_shared<PixelShader>(std::move(PixelShader(shaderDirectory + fileName, entrancePoint, shaderVersion)));
	}


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


	//hull shader
	//---------------------------------------------------------------------------------------------------------------------------------------------------
	void ShaderManager::addHullShader(const std::wstring& fileName, const std::string& entrancePoint, const std::string& shaderVersion)
	{
		assert(hullShaders.find(fileName) == hullShaders.end() && "Hull shader is already created");
		hullShaders[fileName] = std::make_shared<HullShader>(std::move(HullShader(shaderDirectory + fileName, entrancePoint, shaderVersion)));
	}

	std::shared_ptr<HullShader> ShaderManager::getHullShader(const std::wstring& fileName)
	{

		auto value = hullShaders.find(fileName);
		if (value == hullShaders.end())
		{
			std::cerr << "Hull shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
			return nullptr;
		}

		return value->second;
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------



	//domain shader
	//---------------------------------------------------------------------------------------------------------------------------------------------------
	void ShaderManager::addDomainShader(const std::wstring& fileName, const std::string& entrancePoint, const std::string& shaderVersion)
	{
		assert(domainShaders.find(fileName) == domainShaders.end() && "Domain shader is already created");
		domainShaders[fileName] = std::make_shared<DomainShader>(std::move(DomainShader(shaderDirectory + fileName, entrancePoint, shaderVersion)));
	}

	std::shared_ptr<DomainShader> ShaderManager::getDomainShader(const std::wstring& fileName)
	{
		auto value = domainShaders.find(fileName);
		if (value == domainShaders.end())
		{
			std::cerr << "Domain shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
			return nullptr;
		}

		return value->second;
	}
	//---------------------------------------------------------------------------------------------------------------------------------------------------


	
	//geometry shader
	//---------------------------------------------------------------------------------------------------------------------------------------------------

	void engine::DX::ShaderManager::addGeometryShader(const std::wstring& fileName, const std::string& entrancePoint, const std::string& shaderVersion)
	{
		assert(vertexShaders.find(fileName) == vertexShaders.end() && "Geometry shader is already created");
		geometryShaders[fileName] = std::make_shared<GeometryShader>(std::move(GeometryShader(shaderDirectory + fileName, entrancePoint, shaderVersion)));
	}

	std::shared_ptr<GeometryShader> engine::DX::ShaderManager::getGeometryShader(const std::wstring& fileName)
	{
		auto value = geometryShaders.find(fileName);
		if (value == geometryShaders.end())
		{
			std::cerr << "Geometry shader from path: " << fileName.c_str() << " was not found (it's not created)\n";
			return nullptr;
		}

		return value->second;
	}

	//---------------------------------------------------------------------------------------------------------------------------------------------------

}