#include "ReflectionCapture.h"

namespace engine::DX
{

	const std::filesystem::path ReflectionCapture::PRECOMPUTE_PATH = textureDirectory / L"precompute";
	const std::filesystem::path ReflectionCapture::PRECOMPUTE_LUT_PATH = PRECOMPUTE_PATH / L"LUT";
	const std::filesystem::path ReflectionCapture::PRECOMPUTE_IRRADIANCE_PATH = PRECOMPUTE_PATH / L"Irradiance";
	const std::filesystem::path ReflectionCapture::PRECOMPUTE_PREFILTERED_PATH = PRECOMPUTE_PATH / L"Prefiltered";
	const std::wstring ReflectionCapture::LUT_COOK_TORRANCE_FILE_NAME = L"lutCookTorrance.dds";

	static const size_t NUM_CUBE_FACES = 6;
	static const size_t NUM_MIP_MAPS = 5;

	float4x4 ReflectionCapture::GenerateCaptureProjection()
	{
		return float4x4::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(90.0f), 1.0f, 0.1f, 10.0f);
	}

	void ReflectionCapture::InitializeShaders(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc
		{
			{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};
		ShaderManager::getInstance().addVertexShader(vertexShaderFileName, inputElementDesc);
		ShaderManager::getInstance().addPixelShader(pixelShaderFileName);
	}

	SamplerState ReflectionCapture::InitializeSamplerState()
	{
		// Create a sampler state description.
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		// Initialize and bind the sampler state.
		SamplerState samplerState;
		samplerState.initSampleState(0, samplerDesc); // bind to slot 0
		return samplerState;
	}

	D3D11_TEXTURE2D_DESC ReflectionCapture::CreateTextureDesc(
		UINT width,
		UINT height,
		UINT mipLevels,
		UINT arraySize,
		DXGI_FORMAT format,
		UINT miscFlags)
	{
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.MipLevels = mipLevels;
		texDesc.ArraySize = arraySize;
		texDesc.Format = format;
		texDesc.SampleDesc.Count = 1;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.MiscFlags = miscFlags;
		return texDesc;
	}

	D3D11_RENDER_TARGET_VIEW_DESC ReflectionCapture::CreateRenderTargetViewDesc(
		DXGI_FORMAT format,
		D3D11_RTV_DIMENSION viewDimension,
		UINT mipSlice,
		UINT firstArraySlice,
		UINT arraySize)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = format;
		rtvDesc.ViewDimension = viewDimension;
		rtvDesc.Texture2DArray.MipSlice = mipSlice;
		rtvDesc.Texture2DArray.FirstArraySlice = firstArraySlice;
		rtvDesc.Texture2DArray.ArraySize = arraySize;
		return rtvDesc;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC ReflectionCapture::CreateShaderResourceViewDesc(
		DXGI_FORMAT format,
		D3D11_SRV_DIMENSION viewDimension,
		UINT mipLevels,
		UINT mostDetailedMip)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = format;
		srvDesc.ViewDimension = viewDimension;
		srvDesc.TextureCube.MipLevels = mipLevels;
		srvDesc.TextureCube.MostDetailedMip = mostDetailedMip;
		return srvDesc;
	}


	std::array<float4x4, NUM_CUBE_FACES> ReflectionCapture::GenerateCaptureViews()
	{
		return {
			// Right face
			float4x4::CreateLookAt(float3::Zero, float3::UnitX,  float3::UnitY),
			// Left face
			float4x4::CreateLookAt(float3::Zero, -float3::UnitX,  float3::UnitY),
			// Top face
			float4x4::CreateLookAt(float3::Zero,  float3::UnitY,   float3::UnitZ),
			// Bottom face
			float4x4::CreateLookAt(float3::Zero, -float3::UnitY,  -float3::UnitZ),
			// Front face
			float4x4::CreateLookAt(float3::Zero, -float3::UnitZ, float3::UnitY),
			// Back face
			float4x4::CreateLookAt(float3::Zero,  float3::UnitZ,  float3::UnitY)
		};
	}

	void ReflectionCapture::DrawCube()
	{
		vertexBuffer.setBuffer();
		indexBuffer.setBuffer();

		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_devcon->DrawIndexed(36, 0, 0);
	}

	void ReflectionCapture::DrawQuad()
	{
		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		g_devcon->Draw(4, 0);
	}


	Texture2D ReflectionCapture::GenerateIrradianceMap(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& textureFileName, ID3D11ShaderResourceView* environmentCubemap)
	{

		// Save current viewport
		D3D11_VIEWPORT originalViewport;
		UINT numViewports = 1;
		g_devcon->RSGetViewports(&numViewports, &originalViewport);


		InitializeShaders(vertexShaderFileName, pixelShaderFileName);
		SamplerState sampleState = InitializeSamplerState();


		auto& irradianceMapVertexShader = ShaderManager::getInstance().getVertexShader(vertexShaderFileName);
		auto& irradianceMapPixelShader = ShaderManager::getInstance().getPixelShader(pixelShaderFileName);

		auto texDesc = CreateTextureDesc(32, 32, 1, NUM_CUBE_FACES, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_RESOURCE_MISC_TEXTURECUBE);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CreateShaderResourceViewDesc(texDesc.Format, D3D11_SRV_DIMENSION_TEXTURECUBE, texDesc.MipLevels, 0);
		Texture2D irradianceTexture;
		irradianceTexture.createTextureFromMemory(0, texDesc, nullptr, srvDesc);

		auto captureViews = GenerateCaptureViews();
		auto captureProjection = GenerateCaptureProjection();

		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(texDesc.Width);
		viewport.Height = static_cast<float>(texDesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		g_devcon->RSSetViewports(1, &viewport);

		irradianceMapVertexShader->bind();
		irradianceMapPixelShader->bind();
		sampleState.bind();
		g_devcon->PSSetShaderResources(0, 1, &environmentCubemap);

		struct PerView
		{
			float4x4 view;
			float4x4 proj;
		} perView{};

		ConstantBuffer<PerView> perViewBuffer{};
		perViewBuffer.setBufferData({ perView });
		perViewBuffer.initBuffer(0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		perViewBuffer.createBuffer();

		// Create a render target view for each face of the cube map.
		ID3D11RenderTargetView* irradianceRTV[6];
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = CreateRenderTargetViewDesc(texDesc.Format, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, 0, 0, 1);
		HRESULT hr;
		for (int i = 0; i < NUM_CUBE_FACES; ++i) {
			rtvDesc.Texture2DArray.FirstArraySlice = i;
			hr = g_device->CreateRenderTargetView(irradianceTexture.getTexture2DView(), &rtvDesc, &irradianceRTV[i]);
			if (FAILED(hr))
			{
				PrintError(hr, L"Failed to create environment cubemap RTV");
			}
		}

		for (int i = 0; i < NUM_CUBE_FACES; ++i) {

			g_devcon->OMSetRenderTargets(1, &irradianceRTV[i], nullptr);

			float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			g_devcon->ClearRenderTargetView(irradianceRTV[i], clearColor);

			perView.view = captureViews[i].Transpose();
			perView.proj = captureProjection.Transpose();
			perViewBuffer.updateData({ perView });
			perViewBuffer.setVertexShaderBuffer();

			DrawCube();

			ID3D11RenderTargetView* nullRTV[1] = { nullptr };
			g_devcon->OMSetRenderTargets(1, nullRTV, nullptr);
		}


		ID3D11SamplerState* nullSampler[1] = { nullptr };
		g_devcon->PSSetSamplers(0, 1, nullSampler);

		// Restore original viewport
		g_devcon->RSSetViewports(1, &originalViewport);

		return irradianceTexture;
	}


	Texture2D ReflectionCapture::GeneratePrefilteredEnvMap(
		const std::wstring& vertexShaderFileName,
		const std::wstring& pixelShaderFileName,
		const std::wstring& textureFileName,
		ID3D11ShaderResourceView* environmentCubemap)
	{
		// Save current viewport
		D3D11_VIEWPORT originalViewport;
		UINT numViewports = 1;
		g_devcon->RSGetViewports(&numViewports, &originalViewport);

		InitializeShaders(vertexShaderFileName, pixelShaderFileName);
		SamplerState sampleState = InitializeSamplerState();

		auto& prefilteredMapVertexShader = ShaderManager::getInstance().getVertexShader(vertexShaderFileName);
		auto& prefilteredMapPixelShader = ShaderManager::getInstance().getPixelShader(pixelShaderFileName);

		// Texture creation
		auto texDesc = CreateTextureDesc(128, 128, NUM_MIP_MAPS, NUM_CUBE_FACES, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_RESOURCE_MISC_TEXTURECUBE);
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = CreateShaderResourceViewDesc(texDesc.Format, D3D11_SRV_DIMENSION_TEXTURECUBE, texDesc.MipLevels, 0);
		Texture2D prefilteredTexture;
		prefilteredTexture.createTextureFromMemory(0, texDesc, nullptr, srvDesc);

		auto captureViews = GenerateCaptureViews();
		auto captureProjection = GenerateCaptureProjection();

		// Render the pre-filtered environment map by convolving the environment cubemap for varying roughness levels.
		prefilteredMapVertexShader->bind();
		prefilteredMapPixelShader->bind();

		// Set the environment cubemap as the input to the pixel shader.
		g_devcon->PSSetShaderResources(0, 1, &environmentCubemap);

		sampleState.bind();

		ConstantBuffer<float4> roughnessBuffer;
		roughnessBuffer.initBuffer(0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		roughnessBuffer.createBuffer();

		std::array<std::array<ComPtr<ID3D11RenderTargetView>, NUM_MIP_MAPS>, NUM_CUBE_FACES> prefilteredRTV{};
		HRESULT hr;
		for (int mip = 0; mip < NUM_MIP_MAPS; mip++)
		{
			for (int i = 0; i < NUM_CUBE_FACES; ++i)
			{
				auto rtvDesc = CreateRenderTargetViewDesc(texDesc.Format, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, mip, i, 1);
				hr = g_device->CreateRenderTargetView(prefilteredTexture.getTexture2DView(), &rtvDesc, &prefilteredRTV[i][mip]);
				if (FAILED(hr))
				{
					PrintError(hr, L"Failed to create render target view for prefiltered environment map");
				}
			}
		}

		struct PerView
		{
			float4x4 view;
			float4x4 proj;
		} perView{};

		perView.proj = captureProjection.Transpose();

		ConstantBuffer<PerView> perViewBuffer{};
		perViewBuffer.setBufferData({ perView });
		perViewBuffer.initBuffer(0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		perViewBuffer.createBuffer();

		D3D11_VIEWPORT viewport = {};
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		for (int mip = 0; mip < texDesc.MipLevels; mip++)
		{
			float roughness = (float)mip / (float)(texDesc.MipLevels - 1);
			roughnessBuffer.updateData({ {roughness, 0, 0, 0} });

			roughnessBuffer.setPixelShaderBuffer();

			viewport.Width = texDesc.Width * std::pow(0.5, mip);
			viewport.Height = texDesc.Height * std::pow(0.5, mip);

			g_devcon->RSSetViewports(1, &viewport);

			for (int i = 0; i < NUM_CUBE_FACES; ++i)
			{
				perView.view = captureViews[i].Transpose();
				perViewBuffer.updateData({ perView });
				perViewBuffer.setVertexShaderBuffer();

				g_devcon->OMSetRenderTargets(1, prefilteredRTV[i][mip].GetAddressOf(), nullptr);
				static float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				g_devcon->ClearRenderTargetView(prefilteredRTV[i][mip].Get(), clearColor);
				DrawCube();
			}
		}

		// Restore original viewport
		g_devcon->RSSetViewports(1, &originalViewport);

		return prefilteredTexture;
	}

	Texture2D ReflectionCapture::GenerateBRDFLookupTexture(const std::wstring& vertexShaderFileName, const std::wstring& pixelShaderFileName, const std::wstring& textureFileName)
	{
		constexpr size_t BRDFTextureBindSlot = 0;

		// Save current viewport
		D3D11_VIEWPORT originalViewport;
		UINT numViewports = 1;
		g_devcon->RSGetViewports(&numViewports, &originalViewport);

		InitializeShaders(vertexShaderFileName, pixelShaderFileName);
		auto& shaderManager = ShaderManager::getInstance();
		auto& BRDFLookupTextureVertexShader = shaderManager.getVertexShader(vertexShaderFileName);
		auto& BRDFLookupTexturePixelShader = shaderManager.getPixelShader(pixelShaderFileName);

		auto texDesc = CreateTextureDesc(512, 512, 1, 1, DXGI_FORMAT_R16G16_FLOAT);
		auto srvDesc = CreateShaderResourceViewDesc(texDesc.Format, D3D11_SRV_DIMENSION_TEXTURE2D, 1, 0);
		Texture2D BRDFLookupTexture;
		BRDFLookupTexture.createTextureFromMemory(BRDFTextureBindSlot, texDesc, nullptr, srvDesc);

		auto rtvDesc = CreateRenderTargetViewDesc(texDesc.Format, D3D11_RTV_DIMENSION_TEXTURE2D, 0, 0, 0);
		ComPtr<ID3D11RenderTargetView> BRDFLookupTextureRTV;
		HRESULT hr = g_device->CreateRenderTargetView(BRDFLookupTexture.getTexture2DView(), &rtvDesc, BRDFLookupTextureRTV.GetAddressOf());

		//set viewport
		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(texDesc.Width);
		viewport.Height = static_cast<float>(texDesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		g_devcon->RSSetViewports(1, &viewport);

		//bind shaders
		BRDFLookupTextureVertexShader->bind();
		BRDFLookupTexturePixelShader->bind();

		//set render target
		g_devcon->OMSetRenderTargets(1, BRDFLookupTextureRTV.GetAddressOf(), nullptr);

		//clear render target
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		g_devcon->ClearRenderTargetView(BRDFLookupTextureRTV.Get(), clearColor);

		DrawQuad();

		//unset render target
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		g_devcon->OMSetRenderTargets(1, nullRTV, nullptr);

		// Restore original viewport
		g_devcon->RSSetViewports(1, &originalViewport);

		return BRDFLookupTexture;
	}

	

	void ReflectionCapture::SaveTexture(ID3D11Resource* texture, bool generateMips, FileFormat format, const std::wstring& filename) 
	{
		DirectX::ScratchImage scratchImage;
		DirectX::CaptureTexture(g_device, g_devcon, texture, scratchImage);

		const DirectX::ScratchImage* imagePtr = &scratchImage;


		DirectX::ScratchImage mipchain;
		if (generateMips) {
			DirectX::GenerateMipMaps(*scratchImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_DEFAULT, 0, mipchain);
			imagePtr = &mipchain;
		}

		DirectX::ScratchImage compressed;
		if (DirectX::IsCompressed((DXGI_FORMAT)format)) {
			HRESULT result;

			if (FileFormat::BC6_UNSIGNED <= format && format <= FileFormat::BC7_SRGB)
				result = DirectX::Compress(g_device, imagePtr->GetImages(), imagePtr->GetImageCount(), imagePtr->GetMetadata(),
					(DXGI_FORMAT)format, DirectX::TEX_COMPRESS_PARALLEL, 1.f, compressed);
			else
				result = DirectX::Compress(imagePtr->GetImages(), imagePtr->GetImageCount(), imagePtr->GetMetadata(),
					(DXGI_FORMAT)format, DirectX::TEX_COMPRESS_PARALLEL, 1.f, compressed);

			assert(result >= 0);
			imagePtr = &compressed;
		}

		DirectX::SaveToDDSFile(imagePtr->GetImages(), imagePtr->GetImageCount(), imagePtr->GetMetadata(), DirectX::DDS_FLAGS_NONE, filename.c_str());
	}

	/// <summary>
		/// It's important to provide the right textureFileName as it directly affects the generation and retrieval of the irradiance map.
		/// </summary>
	std::shared_ptr<Texture2D> ReflectionCapture::GetIrradianceMap(const std::wstring& textureFileName, UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc)
	{
		std::wstring fullTexturePath = PRECOMPUTE_IRRADIANCE_PATH / textureFileName;


		// If the texture doesn't exist, we need to generate it
		if (!std::filesystem::exists(fullTexturePath))
		{
			//Example:
			//textureFileName - "grass_field_Irradiance.dds"
			//environmentMapFileName - "grass_field.dds"
			//vertexShaderFileName - "IrradianceVertexShader.hlsl"
			//pixelShaderFileName - "IrradiancePixelShader.hlsl"

			// Extracting base name from file
			std::wstring baseName = std::filesystem::path(textureFileName).stem().wstring();

			// Find the "_Irradiance" substring and remove it from the baseName
			std::wstring::size_type pos = baseName.rfind(L"_Irradiance");
			if (pos != std::wstring::npos)
			{
				baseName.erase(pos, baseName.length());
			}

			//currently used only one shader group for convolute the maps (but it's possible to rewrite this function to implement this functionality)
			std::wstring vertexShaderFileName = L"IBL_Precompute/Irradiance/IrradianceVertexShader.hlsl";
			std::wstring pixelShaderFileName = L"IBL_Precompute/Irradiance/IrradiancePixelShader.hlsl";

			std::wstring environmentMapFileName = baseName + L".dds";
			auto environmentCubemapTexture = TextureManager::getInstance().getTexture2D(textureDirectory / L"skybox/hdr" / environmentMapFileName);
			ID3D11ShaderResourceView* environmentCubemapSRV = environmentCubemapTexture->getShaderResourceView();

			Texture2D irradianceTexture = GenerateIrradianceMap(vertexShaderFileName, pixelShaderFileName, textureFileName, environmentCubemapSRV);

			SaveTexture(irradianceTexture.getTexture2DView(), false, FileFormat::BC6_UNSIGNED, fullTexturePath);


		}

		TextureManager::getInstance().addTexture2D(fullTexturePath, bindSlot, textureDesc);
		return TextureManager::getInstance().getTexture2D(fullTexturePath);
	}


	/// <summary>
	/// Providing the correct textureFileName is crucial as it influences the generation and retrieval of the pre-filtered environment map.
	/// <param name="textureDesc">only need to set Usage, BindFlags, CPUAccessFlags and MiscFlags. Don't forget to add D3D11_RESOURCE_MISC_TEXTURECUBE to MiscFlags when adding cubemap texture </param>
	/// </summary>
	std::shared_ptr<Texture2D> ReflectionCapture::GetPrefilteredEnvMap(const std::wstring& textureFileName, UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc)
	{

		std::wstring fullTexturePath = PRECOMPUTE_PREFILTERED_PATH / textureFileName;

		// If the texture doesn't exist, we need to generate it
		if (!std::filesystem::exists(fullTexturePath))
		{
			//Example:
			//textureFileName - "grass_field_Prefiltered.dds"
			//environmentMapFileName - "grass_field.dds"
			//vertexShaderFileName - "PrefilteredVertexShader.hlsl"
			//pixelShaderFileName - "PrefilteredPixelShader.hlsl"

			// Extracting base name from file
			std::wstring baseName = std::filesystem::path(textureFileName).stem().wstring();

			// Find the "_Prefiltered" substring and remove it from the baseName
			std::wstring::size_type pos = baseName.rfind(L"_Prefiltered");
			if (pos != std::wstring::npos)
			{
				baseName.erase(pos, baseName.length());
			}

			//currently used only one shader group for filter the maps (but it's possible to rewrite this function to implement this functionality)
			std::wstring vertexShaderFileName = L"IBL_Precompute/Prefilter/PrefilterVertexShader.hlsl";
			std::wstring pixelShaderFileName = L"IBL_Precompute/Prefilter/PrefilterPixelShader.hlsl";

			std::wstring environmentMapFileName = baseName + L".dds";
			auto environmentCubemapTexture = TextureManager::getInstance().getTexture2D(textureDirectory / L"skybox/hdr" / environmentMapFileName);
			ID3D11ShaderResourceView* environmentCubemapSRV = environmentCubemapTexture->getShaderResourceView();

			Texture2D prefilteredTexture = GeneratePrefilteredEnvMap(vertexShaderFileName, pixelShaderFileName, textureFileName, environmentCubemapSRV);

			SaveTexture(prefilteredTexture.getTexture2DView(), false, FileFormat::BC6_UNSIGNED, fullTexturePath);
		}

		TextureManager::getInstance().addTexture2D(fullTexturePath, bindSlot, textureDesc);
		return TextureManager::getInstance().getTexture2D(fullTexturePath);

	}

	/// <summary>
	/// The correct textureFileName is necessary as it determines the generation and retrieval of the BRDF lookup texture.
	/// </summary>
	std::shared_ptr<Texture2D> ReflectionCapture::GetBRDFLookupTexture(const std::wstring& textureFileName, UINT bindSlot, const D3D11_TEXTURE2D_DESC& textureDesc)
	{
		std::wstring fullTexturePath = PRECOMPUTE_LUT_PATH / textureFileName;

		// If the texture doesn't exist, we need to generate it
		if (!std::filesystem::exists(fullTexturePath))
		{
			// Extracting base name from file
			std::wstring baseName = std::filesystem::path(textureFileName).stem().wstring();

			//currently used only one shader group for generating BRDF LUT maps (but it's possible to rewrite this function to implement this functionality)
			std::wstring vertexShaderFileName = L"IBL_Precompute/LUT/lutVertexShader.hlsl";
			std::wstring pixelShaderFileName = L"IBL_Precompute/LUT/lutPixelShader.hlsl";

			Texture2D BRDFLookupTexture = GenerateBRDFLookupTexture(vertexShaderFileName, pixelShaderFileName, textureFileName);

			SaveTexture(BRDFLookupTexture.getTexture2DView(), false, FileFormat::BC6_UNSIGNED, fullTexturePath);

		}

		TextureManager::getInstance().addTexture2D(fullTexturePath, bindSlot, textureDesc);
		return TextureManager::getInstance().getTexture2D(fullTexturePath);

	}


}