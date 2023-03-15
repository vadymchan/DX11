#pragma once
#include <unordered_map>
#include <string>
#include "../Shader/VertexShader.h"
#include "../Shader/PixelShader.h"
#include "../Shader/HullShader.h"
#include "../Shader/DomainShader.h"
#include "../Shader/GeometryShader.h"

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
		void addVertexShader(const std::wstring& fileName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElementDesc, const std::string& entrancePoint = "main", const std::string& shaderVersion = "vs_5_0");
		bool getVertexShader(const std::wstring& fileName, std::shared_ptr<VertexShader>& vertexShader);
		//analog
		std::shared_ptr<VertexShader> getVertexShader(const std::wstring& fileName);
		//---------------------------------------------------------------------------------------------------------------------------------------------------


		//pixel shader
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		void addPixelShader(const std::wstring& fileName, const std::string& entrancePoint = "main", const std::string& shaderVersion = "ps_5_0");
		bool getPixelShader(const std::wstring& fileName, std::shared_ptr<PixelShader>& pixelShader);
		//analog 
		std::shared_ptr<PixelShader> getPixelShader(const std::wstring& fileName);
		//---------------------------------------------------------------------------------------------------------------------------------------------------


		//hull shader
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		void addHullShader(const std::wstring& fileName, const std::string& entrancePoint = "main", const std::string& shaderVersion = "hs_5_0");
		bool getHullShader(const std::wstring& fileName, std::shared_ptr<HullShader>& geometryShader);
		//analog 
		std::shared_ptr<HullShader> getHullShader(const std::wstring& fileName);
		//---------------------------------------------------------------------------------------------------------------------------------------------------

		//domain shader
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		void addDomainShader(const std::wstring& fileName, const std::string& entrancePoint = "main", const std::string& shaderVersion = "ds_5_0");
		bool getDomainShader(const std::wstring& fileName, std::shared_ptr<DomainShader>& geometryShader);
		//analog 
		std::shared_ptr<DomainShader> getDomainShader(const std::wstring& fileName);
		//---------------------------------------------------------------------------------------------------------------------------------------------------


		//geometry shader
		//---------------------------------------------------------------------------------------------------------------------------------------------------
		void addGeometryShader(const std::wstring& fileName, const std::string& entrancePoint = "main", const std::string& shaderVersion = "gs_5_0");
		bool getGeometryShader(const std::wstring& fileName, std::shared_ptr<GeometryShader>& geometryShader);
		//analog 
		std::shared_ptr<GeometryShader> getGeometryShader(const std::wstring& fileName);
		//---------------------------------------------------------------------------------------------------------------------------------------------------


	private:
		ShaderManager() = default;
		ShaderManager(const ShaderManager&) = delete;
		ShaderManager& operator=(const ShaderManager&) = delete;
		std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>		vertexShaders{};
		std::unordered_map<std::wstring, std::shared_ptr<PixelShader>>		pixelShaders{};
		std::unordered_map<std::wstring, std::shared_ptr<HullShader>>		hullShaders{};
		std::unordered_map<std::wstring, std::shared_ptr<DomainShader>>		domainShaders{};
		std::unordered_map<std::wstring, std::shared_ptr<GeometryShader>>	geometryShaders{};

	};
}
