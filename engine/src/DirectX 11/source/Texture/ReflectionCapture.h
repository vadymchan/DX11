#pragma once

#include <DirectXTex.h>
#include <string>

#include "SampleState.h"
#include "../Controller/TextureManager.h"
#include "../Controller/ShaderManager.h"
#include "../Buffer/VertexBuffer.h"
#include "../Buffer/IndexBuffer.h"
#include "../Buffer/ConstantBuffer.h"
#include "../Controller/BufferManager.h"

namespace engine::DX
{
	class ReflectionCapture
	{
	private:
		struct Vertex 
		{
			DirectX::SimpleMath::Vector3 Pos;
		};

		std::vector<Vertex> vertices = {
			{ DirectX::SimpleMath::Vector3(-1.0f, 1.0f, -1.0f)},
			{ DirectX::SimpleMath::Vector3(-1.0f, 1.0f,  1.0f)},
			{ DirectX::SimpleMath::Vector3(-1.0f, -1.0f, -1.0f)},
			{ DirectX::SimpleMath::Vector3(-1.0f, -1.0f,  1.0f)},
			{ DirectX::SimpleMath::Vector3(1.0f, 1.0f, -1.0f)},
			{ DirectX::SimpleMath::Vector3(1.0f, 1.0f,  1.0f)},
			{ DirectX::SimpleMath::Vector3(1.0f, -1.0f, -1.0f)},
			{ DirectX::SimpleMath::Vector3(1.0f, -1.0f,  1.0f)},
		};

		std::vector<UINT> indices = {
			2, 3, 1,    0, 2, 1, // left
			4, 5, 6,    5, 7, 6, // right
			1, 3, 5,    3, 7, 5, // forward
			0, 4, 2,    4, 6, 2, // back
			0, 1, 4,    1, 5, 4, // up
			2, 6, 3,    6, 7, 3, // bottom
		};

	public:

		static const std::filesystem::path PRECOMPUTE_PATH;
		static const std::filesystem::path PRECOMPUTE_LUT_PATH;
		static const std::filesystem::path PRECOMPUTE_IRRADIANCE_PATH;
		static const std::filesystem::path PRECOMPUTE_PREFILTERED_PATH;
		static const std::wstring LUT_COOK_TORRANCE_FILE_NAME;



		enum class FileFormat
		{
			NONE,
			PNG,
			TGA,
			HDR,
			BC1_LINEAR = DXGI_FORMAT_BC1_UNORM,			// RGB, 1 bit Alpha
			BC1_SRGB = DXGI_FORMAT_BC1_UNORM_SRGB,		// RGB, 1-bit Alpha, SRGB
			BC3_LINEAR = DXGI_FORMAT_BC3_UNORM,			// RGBA
			BC3_SRGB = DXGI_FORMAT_BC3_UNORM_SRGB,		// RGBA, SRGB
			BC4_UNSIGNED = DXGI_FORMAT_BC4_UNORM,		// GRAY, unsigned
			BC4_SIGNED = DXGI_FORMAT_BC4_SNORM,			// GRAY, signed
			BC5_UNSIGNED = DXGI_FORMAT_BC5_UNORM,		// RG, unsigned
			BC5_SIGNED = DXGI_FORMAT_BC5_SNORM,			// RG, signed
			BC6_UNSIGNED = DXGI_FORMAT_BC6H_UF16,		// RGB HDR, unsigned
			BC6_SIGNED = DXGI_FORMAT_BC6H_SF16,			// RGB HDR, signed
			BC7_LINEAR = DXGI_FORMAT_BC7_UNORM,			// RGBA Advanced
			BC7_SRGB = DXGI_FORMAT_BC7_UNORM_SRGB,		// RGBA Advanced, SRGB
		};


		static ReflectionCapture& getInstance()
		{
			static ReflectionCapture instance;
			return instance;
		}

		Texture2D  GenerateIrradianceMap(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& textureFileName, std::shared_ptr<Texture2D> environmentCubemap);
		Texture2D  GeneratePrefilteredEnvMap(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& textureFileName, std::shared_ptr<Texture2D> environmentCubemap);
		Texture2D  GenerateBRDFLookupTexture(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& textureFileName);
		void SaveTexture(ID3D11Resource* texture, bool generateMips, FileFormat format, const std::wstring& filename);

		/// <summary>
		/// It's important to provide the right textureFileName as it directly affects the generation and retrieval of the irradiance map.
		/// <param name="textureDesc">only need to set Usage, BindFlags, CPUAccessFlags and MiscFlags. Don't forget to add D3D11_RESOURCE_MISC_TEXTURECUBE to MiscFlags when adding cubemap texture </param>
		/// </summary>
		std::shared_ptr<Texture2D> GetIrradianceMap(const std::wstring& textureFileName, UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc);
		
		/// <summary>
		/// Providing the correct textureFileName is crucial as it influences the generation and retrieval of the pre-filtered environment map.
		/// <param name="textureDesc">only need to set Usage, BindFlags, CPUAccessFlags and MiscFlags. Don't forget to add D3D11_RESOURCE_MISC_TEXTURECUBE to MiscFlags when adding cubemap texture </param>
		/// </summary>
		std::shared_ptr<Texture2D> GetPrefilteredEnvMap(const std::wstring& textureFileName, UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc);

		/// <summary>
		/// The correct textureFileName is necessary as it determines the generation and retrieval of the BRDF lookup texture.
		/// <param name="textureDesc">only need to set Usage, BindFlags, CPUAccessFlags and MiscFlags. </param>
		/// </summary>
		std::shared_ptr<Texture2D> GetBRDFLookupTexture(const std::wstring& textureFileName, UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc);

		//utility function
		D3D11_TEXTURE2D_DESC CreateTextureDesc(UINT width, UINT height, UINT mipLevels, UINT arraySize, DXGI_FORMAT format, UINT miscFlags = 0);

	private:

		ReflectionCapture()
		{
			// Initialize the vertex and index buffers.
			vertexBuffer.initBuffer(0, { sizeof(Vertex) }, { 0 }, D3D11_USAGE_DEFAULT);
			indexBuffer.initBuffer(D3D11_USAGE_DEFAULT);

			vertexBuffer.setBufferData(vertices);
			indexBuffer.setBufferData(indices);
		}
		ReflectionCapture(const ReflectionCapture&) = delete;
		ReflectionCapture& operator=(const ReflectionCapture&) = delete;
		ReflectionCapture(ReflectionCapture&&) = delete;
		ReflectionCapture& operator=(ReflectionCapture&&) = delete;

		VertexBuffer<Vertex> vertexBuffer;
		IndexBuffer indexBuffer;

		//BEGIN: utility functions
		//------------------------
		D3D11_SHADER_RESOURCE_VIEW_DESC CreateShaderResourceViewDesc(DXGI_FORMAT format, D3D11_SRV_DIMENSION viewDimension, UINT mipLevels, UINT mostDetailedMip);
		D3D11_RENDER_TARGET_VIEW_DESC CreateRenderTargetViewDesc(DXGI_FORMAT format, D3D11_RTV_DIMENSION viewDimension, UINT mipSlice, UINT firstArraySlice, UINT arraySize);
		int ComputeMipMapCount(int width, int height);

		float4x4 GenerateCaptureProjection();

		void InitializeShaders(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName);

		SamplerState InitializeSamplerState();

		std::array<float4x4, 6> GenerateCaptureViews();

		void DrawCube();
		void DrawQuad();
		//END: utility functions
		//----------------------
	};

}

