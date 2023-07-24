#include "ApplicationDX.h"
#include <random>

using Instance = engine::DX::OpaqueInstances::Instance;
using OpaqueMaterial = engine::DX::OpaqueInstances::Material;
using DissolutionMaterial = engine::DX::DissolutionInstances::Material;
using Attenuation = engine::DX::LightSystem::Attenuation;
using PointLight = engine::DX::LightSystem::PointLight;
using BaseLight = engine::DX::LightSystem::BaseLight;
using SpotLight = engine::DX::LightSystem::SpotLight;
using DirectionalLight = engine::DX::LightSystem::DirectionalLight;
using engine::DX::Model;
using engine::DX::Mesh;
using engine::DX::MeshSystem;
using engine::DX::ModelManager;
using engine::DX::ShaderManager;
using engine::DX::TextureManager;
using engine::DX::LightSystem;
using engine::DX::ParticleSystem;
using engine::DX::Emitter;
using engine::DX::Texture2D;

void ApplicationDX::Init(const HINSTANCE& appHandle, int windowShowParams)
{
	const int windowWidth{ 1000 };
	const int windowHeight{ 1000 };
	const int windowStartX{ 0 };
	const int windowStartY{ 0 };

	const std::wstring normalVertexShaderFileName{ L"normalVisualiser/normalVisualizerVertexShader.hlsl" };
	const std::wstring normalGeometryShaderFileName{ L"normalVisualiser/normalVisualizerGeometryShader.hlsl" };
	const std::wstring normalPixelShaderFileName{ L"normalVisualiser/normalVisualizerPixelShader.hlsl" };

	const std::wstring colorVertexShaderFileName{ L"texture/colorVertexShader.hlsl" };
	const std::wstring colorPixelShaderFileName{ L"texture/colorPixelShader.hlsl" };

	const std::wstring hologramVertexShaderFileName{ L"hologram/hologramVertexShader.hlsl" };
	const std::wstring hologramGeometryShaderFileName{ L"hologram/hologramGeometryShader.hlsl" };
	const std::wstring hologramHullShaderFileName{ L"hologram/hologramHullShader.hlsl" };
	const std::wstring hologramDomainShaderFileName{ L"hologram/hologramDomainShader.hlsl" };
	const std::wstring hologramPixelShaderFileName{ L"hologram/hologramPixelShader.hlsl" };

	const std::wstring pointLightVertexShaderFileName{ L"pointLight/pointLightVertexShader.hlsl" };
	const std::wstring pointLightPixelShaderFileName{ L"pointLight/pointLightPixelShader.hlsl" };

	const std::wstring blinnPhongVertexShaderFileName{ L"Blinn-Phong/Blinn-PhongVertexShader.hlsl" };
	const std::wstring blinnPhongPixelShaderFileName{ L"Blinn-Phong/Blinn-PhongPixelShader.hlsl" };

	const std::wstring pbrVertexShaderFileName{ L"PBR/PBR_VertexShader.hlsl" };
	const std::wstring pbrPixelShaderFileName{ L"PBR/PBR_PixelShader.hlsl" };

	const std::wstring iblVertexShaderFileName{ L"IBL/IBL_VertexShader.hlsl" };
	const std::wstring iblPixelShaderFileName{ L"IBL/IBL_PixelShader.hlsl" };

	const std::wstring iblShadowVertexShaderFileName{ L"IBL/IBL_Shadow_VertexShader.hlsl" };
	const std::wstring iblShadowPixelShaderFileName{ L"IBL/IBL_Shadow_PixelShader.hlsl" };

	const std::wstring shadowMapVertexShaderFileName{ L"Shadow/2D/shadow2DVertexShader.hlsl" };
	const std::wstring shadowMapPixelShaderFileName{ L"Shadow/2D/shadow2DPixelShader.hlsl" };

	const std::wstring shadowCubeMapVertexShaderFileName{ L"Shadow/Omnidirectional/shadow3DVertexShader.hlsl" };
	const std::wstring shadowCubeMapGeometryShaderFileName{ L"Shadow/Omnidirectional/shadow3DGeometryShader.hlsl" };
	const std::wstring shadowCubeMapPixelShaderFileName{ L"Shadow/Omnidirectional/shadow3DPixelShader.hlsl" };

	const std::wstring dissolutionShadowMapVertexShaderFileName{ L"Shadow/2D/shadow2DDissolutionVertexShader.hlsl" };
	const std::wstring dissolutionShadowMapPixelShaderFileName{ L"Shadow/2D/shadow2DDissolutionPixelShader.hlsl" };

	const std::wstring dissolutionShadowCubeMapVertexShaderFileName{ L"Shadow/Omnidirectional/shadow3DDissolutionVertexShader.hlsl" };
	const std::wstring dissolutionShadowCubeMapGeometryShaderFileName{ L"Shadow/Omnidirectional/shadow3DDissolutionGeometryShader.hlsl" };
	const std::wstring dissolutionShadowCubeMapPixelShaderFileName{ L"Shadow/Omnidirectional/shadow3DDissolutionPixelShader.hlsl" };

	const std::wstring skyboxNightStreet{ engine::DX::textureDirectory / L"skybox/hdr" / "night_street.dds" };
	const std::wstring skyboxGrassField{ engine::DX::textureDirectory / L"skybox/hdr" / "grass_field.dds" };

	const std::wstring flashLight{ engine::DX::textureDirectory / L"flashLight/dragonslake.dds" };

	const std::wstring iblDissolutionVertexShaderFileName{ L"IBL/IBL_Dissolution_VertexShader.hlsl" };
	const std::wstring iblDissolutionPixelShaderFileName{ L"IBL/IBL_Dissolution_PixelShader.hlsl" };

	const std::wstring smokeParticleVertexShaderFileName{ L"Particles/Particles_VertexShader.hlsl" };
	const std::wstring smokeParticlePixelShaderFileName{ L"Particles/Particles_PixelShader.hlsl" };

	cameraSpeed = 2.f;
	cameraRotationSpeed = 0.005f;
	cameraMaxPitch = DirectX::XMConvertToRadians(89.0f);
	cameraMinPitch = DirectX::XMConvertToRadians(-89.0f);
	wireframeMode = false;
	deltaExposure = 1.f;
	flashLightAttached = true;
	dissolutionEffectDuration = 5000;


	// compute integral of cosine over the hemisphere using Fibonacci hemisphere point distribution
	// --------------------------------------------------------------------------------------------
	std::cout << "Integral over the hemisphere: " << engine::hemisphereIntegral(1000000) << std::endl;
	// --------------------------------------------------------------------------------------------




	engine.initWindow("DirectX 11 Model", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = wireframeMode ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	//rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.DepthBias = -1; // adjust as needed (for depth bias)
	rasterizerDesc.SlopeScaledDepthBias = -1; // adjust as needed (for depth bias)
	rasterizerDesc.DepthBiasClamp = 0.0f;

	engine::DX::Skybox skybox;
	skybox.initSkybox(skyboxGrassField);

	engine.initRenderer(rasterizerDesc, std::make_shared<engine::DX::Skybox>(skybox));
	//shaders & input layout
	// -------------------------------------------------------------------------------------------------
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc
	{
		//model vertex buffer
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(2), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescNormal
	{
		//model vertex buffer
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(2), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> pbrInputElementDesc
	{
		//model vertex buffer
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(2), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(3), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(4), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},

	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> pointLightInputElementDesc
	{
		// per-vertex data
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		// per-instance data
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 16 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 32 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 48 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"EMISSION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 128 , D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};


	std::vector<D3D11_INPUT_ELEMENT_DESC> hologramInputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> shadowInputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> dissolutionInputElementDesc
	{
		//model vertex buffer
		{"POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(2), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT",   0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(3), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(4), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INVINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 112, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TIME",     0, DXGI_FORMAT_R32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 128,  D3D11_INPUT_PER_INSTANCE_DATA, 1},


	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> dissolutionShadowInputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(2), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"TIME",     0, DXGI_FORMAT_R32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 128,  D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> particleInputElementDesc
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::PARTICLE_DATA_INPUT_SLOT_2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::PARTICLE_DATA_INPUT_SLOT_2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::PARTICLE_DATA_INPUT_SLOT_2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"LIFETIME", 0, DXGI_FORMAT_R32_FLOAT,		 engine::DX::PARTICLE_DATA_INPUT_SLOT_2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"ROTATION", 0, DXGI_FORMAT_R32_FLOAT,       engine::DX::PARTICLE_DATA_INPUT_SLOT_2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    engine::DX::PARTICLE_DATA_INPUT_SLOT_2, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	auto& shaderManager = ShaderManager::getInstance();

	//vertex shader
	shaderManager.addVertexShader(normalVertexShaderFileName, inputElementDescNormal);
	shaderManager.addVertexShader(colorVertexShaderFileName, inputElementDesc);
	shaderManager.addVertexShader(blinnPhongVertexShaderFileName, inputElementDescNormal);
	shaderManager.addVertexShader(pbrVertexShaderFileName, pbrInputElementDesc);
	shaderManager.addVertexShader(iblVertexShaderFileName, pbrInputElementDesc);
	shaderManager.addVertexShader(hologramVertexShaderFileName, hologramInputElementDesc);
	shaderManager.addVertexShader(pointLightVertexShaderFileName, pointLightInputElementDesc);
	shaderManager.addVertexShader(shadowMapVertexShaderFileName, shadowInputElementDesc);
	shaderManager.addVertexShader(shadowCubeMapVertexShaderFileName, shadowInputElementDesc);
	shaderManager.addVertexShader(iblShadowVertexShaderFileName, pbrInputElementDesc);
	shaderManager.addVertexShader(iblDissolutionVertexShaderFileName, dissolutionInputElementDesc);
	shaderManager.addVertexShader(dissolutionShadowMapVertexShaderFileName, dissolutionShadowInputElementDesc);
	shaderManager.addVertexShader(dissolutionShadowCubeMapVertexShaderFileName, dissolutionShadowInputElementDesc);
	shaderManager.addVertexShader(smokeParticleVertexShaderFileName, particleInputElementDesc);
	//hull shader
	shaderManager.addHullShader(hologramHullShaderFileName);
	//domain shader
	shaderManager.addDomainShader(hologramDomainShaderFileName);
	//geometry shader
	shaderManager.addGeometryShader(normalGeometryShaderFileName);
	shaderManager.addGeometryShader(hologramGeometryShaderFileName);
	shaderManager.addGeometryShader(shadowCubeMapGeometryShaderFileName);
	shaderManager.addGeometryShader(dissolutionShadowCubeMapGeometryShaderFileName);
	//pixel shader
	shaderManager.addPixelShader(normalPixelShaderFileName);
	shaderManager.addPixelShader(colorPixelShaderFileName);
	shaderManager.addPixelShader(hologramPixelShaderFileName);
	shaderManager.addPixelShader(blinnPhongPixelShaderFileName);
	shaderManager.addPixelShader(pointLightPixelShaderFileName);
	shaderManager.addPixelShader(pbrPixelShaderFileName);
	shaderManager.addPixelShader(iblPixelShaderFileName);
	shaderManager.addPixelShader(shadowMapPixelShaderFileName);
	shaderManager.addPixelShader(shadowCubeMapPixelShaderFileName);
	shaderManager.addPixelShader(dissolutionShadowMapPixelShaderFileName);
	shaderManager.addPixelShader(dissolutionShadowCubeMapPixelShaderFileName);
	shaderManager.addPixelShader(iblShadowPixelShaderFileName);
	shaderManager.addPixelShader(iblDissolutionPixelShaderFileName);
	shaderManager.addPixelShader(smokeParticlePixelShaderFileName);

	//textures & sampler 
	//---------------------------------------------------------------------------------------------------

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// opaque instance
	//---------------------------------------------------------------------------------------------------

	//uint32_t normalOpaqueInstance = engine.createOpaqueInstance({ {blinnPhongVertexShaderFileName, L"", L"", L"",blinnPhongPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	//uint32_t pbrOpaqueInstance = engine.createOpaqueInstance({ {pbrVertexShaderFileName, L"", L"", L"",pbrPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	iblOpaqueInstance = engine.createOpaqueInstance({ {iblShadowVertexShaderFileName, L"", L"", L"",iblShadowPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	sphereOpaqueInstance = engine.createOpaqueInstance({ {pointLightVertexShaderFileName, L"", L"", L"",pointLightPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	//uint32_t hologramOpaqueInstance = engine.createOpaqueInstance({ {hologramVertexShaderFileName, L"", L"", /*hologramHullShaderFileName, hologramDomainShaderFileName,*/ hologramGeometryShaderFileName, hologramPixelShaderFileName}, /*{normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName, normalPixelShaderFileName},*/ });

	// dissolution instance
	dissolutionInstancesID = engine.createDissolutionInstance({ { iblDissolutionVertexShaderFileName, L"", L"", L"",iblDissolutionPixelShaderFileName } });

	//light
	//---------------------------------------------------------------------------------------------------

	//point light 

	std::vector<engine::DX::float4> pointLightEmissions
	{
		{1, 1, 1, 0},
		{1, 1, 1, 0},
	};

	std::vector<engine::DX::float3> pointLightPositions = {
		{5, 5, -2},
		{-5, 5, -4},

	};



	std::vector<PointLight> pointLights
	{

		PointLight
		{
			{ {1, 1, 1}, 1000 },
			{}, // will be updated after the insertion to the instanceGroup 
			0.5,
			{ 1.0, 0.045, 0.0075 }
		},
		PointLight
		{
			{ {1, 1, 1}, 1000 },
			{}, // will be updated after the insertion to the instanceGroup 
			0.5,
			{ 1.0, 0.045, 0.0075 }
		},

	};



	//spot light 

	std::vector<engine::DX::float4> spotLightEmissions{ {1,0,0,0}, {1,0,0,0}, };
	std::vector<engine::DX::float3> spotLightPositions{ {5,1.5, -3},  {-5,1.5, -3}, };


	std::vector<SpotLight> spotLights
	{
		SpotLight
		{
			{ {1, 0, 0}, 1000 },
			{}, // will be updated after the insertion to the instanceGroup 
			DirectX::XMConvertToRadians(30.0f),
			DirectX::XMConvertToRadians(30.0f),
			0.5,
			{0,0,1},
			{ 1.0, 0.045, 0.0075 }
		},
		SpotLight
		{
			{ {1, 0, 0}, 1000 },
			{}, // will be updated after the insertion to the instanceGroup 
			DirectX::XMConvertToRadians(30.0f),
			DirectX::XMConvertToRadians(30.0f),
			0.5,
			{0,0,1},
			{ 1.0, 0.045, 0.0075 }
		},
	};


	//directional light 

	std::vector<DirectionalLight> directionalLights
	{
		DirectionalLight
		{
			{{ 1, 1, 1 }, 10, },
			{ 0, -0.707, -0.707 },
			1
		},

	};

	// models
	//---------------------------------------------------------------------------------------------------
	std::shared_ptr<Model> cube = ModelManager::getInstance().getModel(ModelManager::cubeTag);
	std::shared_ptr<Model> sphere = ModelManager::getInstance().getModel(ModelManager::sphereTag);
	std::shared_ptr<Model> floor = ModelManager::getInstance().getModel(ModelManager::floorTag);
	std::shared_ptr<Model> flatSphere = ModelManager::getInstance().getModel(ModelManager::flatSphereTag);
	std::shared_ptr<Model> samurai = ModelManager::getInstance().getModel("Samurai/Samurai.fbx");
	std::shared_ptr<Model> eastTower = ModelManager::getInstance().getModel("EastTower/EastTower.fbx");
	std::shared_ptr<Model> knight = ModelManager::getInstance().getModel("Knight/Knight.fbx");
	std::shared_ptr<Model> knightHorse = ModelManager::getInstance().getModel("KnightHorse/KnightHorse.fbx");

	spawnModel = knight;



	std::vector<engine::DX::float4x4> samuraiInstance
	{
			 engine::DX::float4x4 {
				{3,0,0,2},
				{0,3,0,-1},
				{0,0,3,3},
				{0,0,0,1},
			} ,
	};


	std::vector<engine::DX::float4x4> eastTowerInstances
	{
			engine::DX::float4x4
			{{{1,0,0,5},
			{0,1,0,-1},
			{0,0,1,3},
			{0,0,0,1},}},
	};

	std::vector<engine::DX::float4x4> knightInstances
	{
			engine::DX::float4x4
			{{{2,0,0,-2},
			{0,2,0,-1},
			{0,0,2,3},
			{0,0,0,1},}}
		,
	};

	std::vector<engine::DX::float4x4> knightHorseInstances
	{

			engine::DX::float4x4
			{{{1,0,0,-4},
			{0,1,0,-1},
			{0,0,1,3},
			{0,0,0,1},}},
	};

	std::vector<engine::DX::float4x4> spherePointLightInstances;
	for (int i = 0; i < pointLights.size(); i++)
	{
		spherePointLightInstances.emplace_back(engine::DX::float4x4
			{
				{pointLights[i].radius, 0, 0, pointLightPositions[i].x},
				{0, pointLights[i].radius, 0, pointLightPositions[i].y},
				{0, 0, pointLights[i].radius, pointLightPositions[i].z},
				{0, 0, 0, 1}
			});
	}

	std::vector<engine::DX::float4x4> sphereSpotLightInstances;
	for (int i = 0; i < spotLights.size(); i++)
	{
		sphereSpotLightInstances.emplace_back(engine::DX::float4x4
			{
				{spotLights[i].radius, 0, 0, spotLightPositions[i].x},
				{0, spotLights[i].radius, 0, spotLightPositions[i].y},
				{0, 0, spotLights[i].radius, spotLightPositions[i].z},
				{0, 0, 0, 1}
			});
	}




	std::vector<engine::DX::float4x4> floorInstances
	{

		engine::DX::float4x4
		{{
			{100,0,0,0},
			{0,1,0, -1},
			{0,0,100,0},
			{0,0,0,1},
		}},

		//floor rotated 90 degrees
		/*engine::DX::float4x4
		{{
			{100,0,0,0},
			{0,0,100, -1},
			{0,-1,0,0},
			{0,0,0,1},
		}},*/

	};


	std::vector<size_t> cubeMeshIndices;
	for (size_t i = 0; i < cube.get()->getMeshesCount(); ++i)
	{
		cubeMeshIndices.emplace_back(i);
	}

	std::vector<engine::DX::float4x4> mirrorSphere
	{
		engine::DX::float4x4
		{{{1,0,0, 0},
		  {0,1,0, 0},
		  {0,0,1, 1},
		  {0,0,0, 1},}},
	};

	std::vector<engine::DX::float4x4> shinyConductorSphere
	{
		engine::DX::float4x4
		{{{1,0,0, 2},
		  {0,1,0, 0},
		  {0,0,1, 1},
		  {0,0,0, 1},}},
	};

	std::vector<engine::DX::float4x4> roughDielectricSphere
	{
		engine::DX::float4x4
		{{{1,0,0, -2},
		  {0,1,0, 0},
		  {0,0,1, 1},
		  {0,0,0, 1},}},
	};

	opaqueMaterials[DIAMOND] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"diamond" });
	opaqueMaterials[OBSIDIAN] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"obsidian" });
	opaqueMaterials[GRASS] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"Grass" });
	opaqueMaterials[PEACOCK] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"Peacock" });
	opaqueMaterials[VINES] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"Vines" });
	opaqueMaterials[NORMAL_DEBUG] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"normalDebug" });
	opaqueMaterials[MIRROR] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"Mirror" });
	opaqueMaterials[SHINY_CONDUCTOR] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"Shiny Conductor" });
	opaqueMaterials[ROUGH_DIELECTRIC] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"Rough Dielectric" });
	opaqueMaterials[NO_MATERIAL] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"" });
	opaqueMaterials[DEFAULT_SKIN] = std::make_shared<OpaqueMaterial>(OpaqueMaterial{ L"default" });


	dissolutionMaterials[DIAMOND] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"diamond" });
	dissolutionMaterials[OBSIDIAN] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"obsidian" });
	dissolutionMaterials[GRASS] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"Grass" });
	dissolutionMaterials[PEACOCK] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"Peacock" });
	dissolutionMaterials[VINES] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"Vines" });
	dissolutionMaterials[NORMAL_DEBUG] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"normalDebug" });
	dissolutionMaterials[MIRROR] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"Mirror" });
	dissolutionMaterials[SHINY_CONDUCTOR] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"Shiny Conductor" });
	dissolutionMaterials[ROUGH_DIELECTRIC] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"Rough Dielectric" });
	dissolutionMaterials[NO_MATERIAL] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"" });
	dissolutionMaterials[DEFAULT_SKIN] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"default" });
	dissolutionMaterials[DISSOLUTION_SKIN] = std::make_shared<DissolutionMaterial>(DissolutionMaterial{ L"dissolution" });

	//engine.addOpaqueInstances(iblOpaqueInstance, sphere, cubeMeshIndices, opaqueMaterials[MIRROR], mirrorSphere);
	//engine.addOpaqueInstances(iblOpaqueInstance, sphere, cubeMeshIndices, opaqueMaterials[SHINY_CONDUCTOR], shinyConductorSphere);
	//engine.addOpaqueInstances(iblOpaqueInstance, sphere, cubeMeshIndices, opaqueMaterials[ROUGH_DIELECTRIC], roughDielectricSphere);


	engine.addOpaqueInstances(iblOpaqueInstance, floor, { 0 }, opaqueMaterials[GRASS], floorInstances);

	std::vector<size_t> samuraiMeshIndices;
	for (size_t i = 0; i < samurai.get()->getMeshesCount(); ++i)
	{
		samuraiMeshIndices.emplace_back(i);
	}

	//engine.addOpaqueInstances(iblOpaqueInstance, samurai, samuraiMeshIndices, opaqueMaterials[DEFAULT_SKIN], samuraiInstance);


	std::vector<size_t> sphereMeshIndeices;
	for (size_t i = 0; i < sphere.get()->getMeshesCount(); ++i)
	{
		sphereMeshIndeices.emplace_back(i);
	}

	std::vector<engine::DX::TransformSystem::ID> pointLightInstanceIDs =
		engine.addOpaqueInstances(sphereOpaqueInstance, sphere, sphereMeshIndeices, opaqueMaterials[NO_MATERIAL], spherePointLightInstances, pointLightEmissions);
	
	std::vector<engine::DX::TransformSystem::ID> spotLightInstanceIDs =
		engine.addOpaqueInstances(sphereOpaqueInstance, sphere, sphereMeshIndeices, opaqueMaterials[NO_MATERIAL], sphereSpotLightInstances, spotLightEmissions);


	std::vector<size_t> eastTowerMeshIndices;
	for (size_t i = 0; i < eastTower.get()->getMeshesCount(); ++i)
	{
		eastTowerMeshIndices.emplace_back(i);
	}
	//engine.addOpaqueInstances(iblOpaqueInstance, eastTower, eastTowerMeshIndices, opaqueMaterials[DEFAULT_SKIN], eastTowerInstances);


	std::vector<size_t> knightMeshIndices;
	for (size_t i = 0; i < knight.get()->getMeshesCount(); ++i)
	{
		knightMeshIndices.emplace_back(i);
	}
	//engine.addOpaqueInstances(iblOpaqueInstance, knight, knightMeshIndices, opaqueMaterials[DEFAULT_SKIN], knightInstances);

	std::vector<size_t> knightHorseMeshIndices;
	for (size_t i = 0; i < knightHorse.get()->getMeshesCount(); ++i)
	{
		knightHorseMeshIndices.emplace_back(i);
	}
	//engine.addOpaqueInstances(iblOpaqueInstance, knightHorse, knightHorseMeshIndices, opaqueMaterials[DEFAULT_SKIN], knightHorseInstances);

	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const engine::DX::float3 position{ 0, 0, 0 };
	engine::DX::float3 direction{ 0, 0, 1 };
	direction.Normalize();
	const engine::DX::float3 worldUp{ 0, 1, 0 };


	float fov{ 60 };
	float aspect{ (float)windowWidth / windowHeight };
	float zNear{ 0.01 };
	float zFar{ 30 };

	engine.initCamera(position, direction, worldUp, fov, aspect, zNear, zFar);

	// light 
	//---------------------------------------------------------------------------------------------------



	LightSystem& lightSystem = LightSystem::getInstance();

	for (size_t i = 0; i < directionalLights.size(); i++)
	{
		lightSystem.addDirectionalLight(directionalLights[i]);

	}


	//cameraFlashLight = lightSystem.addFlashLight(
	//	{ 1,1,1 },
	//	1000,
	//	DirectX::XMConvertToRadians(10.0f),
	//	DirectX::XMConvertToRadians(15.0f),
	//	0.1,
	//	pointLightAttenuations[0],
	//	flashLight
	//);

	for (size_t i = 0; i < spotLightInstanceIDs.size(); i++)
	{
		spotLights[i].transformID = spotLightInstanceIDs[i];
		lightSystem.addSpotLight(spotLights[i]);
	}
	
	
	for (int i = 0; i < pointLightInstanceIDs.size(); ++i)
	{
	
		pointLights[i].transformID = pointLightInstanceIDs[i];
		lightSystem.addPointLight(pointLights[i]);
	
	};


	// particles
	//---------------------------------------------------------------------------------------------------

	ParticleSystem& particleSystem = engine.getParticleSystem();

	particleSystem.setShaders(
		shaderManager.getVertexShader(smokeParticleVertexShaderFileName),
		shaderManager.getPixelShader(smokeParticlePixelShaderFileName));

	auto& textureManager = TextureManager::getInstance();

	const auto smokeTextureDirectory = engine::DX::textureDirectory / L"smoke";
	const std::wstring smokeRLUfile = smokeTextureDirectory / L"smoke_RLU.dds";
	const std::wstring smokeDBFfile = smokeTextureDirectory / L"smoke_DBF.dds";
	const std::wstring smokeMVEAfile = smokeTextureDirectory / L"smoke_MVEA.dds";

	textureManager.addTexture2D(smokeRLUfile, engine::DX::SMOKE_RLU_TEXTURE_BIND_SLOT, textureDesc);
	textureManager.addTexture2D(smokeDBFfile, engine::DX::SMOKE_DBF_TEXTURE_BIND_SLOT, textureDesc);
	textureManager.addTexture2D(smokeMVEAfile, engine::DX::SMOKE_MVEA_TEXTURE_BIND_SLOT, textureDesc);

	std::array<std::weak_ptr<Texture2D>, (size_t)ParticleSystem::TextureType::Count> particleTextures;
	particleTextures[(int)ParticleSystem::TextureType::RLU] = textureManager.getTexture2D(smokeRLUfile);
	particleTextures[(int)ParticleSystem::TextureType::DBF] = textureManager.getTexture2D(smokeDBFfile);
	particleTextures[(int)ParticleSystem::TextureType::MVEA] = textureManager.getTexture2D(smokeMVEAfile);

	particleSystem.setParticleTextures(particleTextures);

	std::vector<Emitter> smokeEmitters = {
	{
		{0.0f, 0.0f, 5.0f}, // position
		100, // spawn rate
		0, // spawn radius
		{0,1,1, 1.0}, // particle color
		1.0f, // bounding sphere radius
		//10.f, // initial particle size
		0.1f, // initial particle size
		0.0f, // initial rotation angle
		5.0f, // vertical speed
		5, // random speed range
		0.4f, // random speed scale
		100, // rotation speed range
		0.002f // rotation speed scale
	},
	{
		{4.0f, 0.0f, 5.0f}, // position
		200, // spawn rate
		0, // spawn radius
		{1,1,0, 1.0}, // particle color
		1.0f, // bounding sphere radius
		0.1f, // initial particle size
		0.0f, // initial rotation angle
		2.0f, // vertical speed
		5, // random speed range
		0.4f, // random speed scale
		100, // rotation speed range
		0.2f // rotation speed scale
	},
	{
		{-4.0f, 0.0f, 5.0f}, // position
		200, // spawn rate
		0, // spawn radius
		{1,1,1, 1.0}, // particle color
		1.0f, // bounding sphere radius
		0.1f, // initial particle size
		0.0f, // initial rotation angle
		2.0f, // vertical speed
		5, // random speed range
		0.4f, // random speed scale
		100, // rotation speed range
		0.2f // rotation speed scale
	}
	};

	std::for_each(smokeEmitters.begin(), smokeEmitters.end(), [&particleSystem](const Emitter& smokeEmitter) {
		particleSystem.addSmokeEmitter(smokeEmitter);
	});


	//

	//---------------------------------------------------------------------------------------------------
}

void ApplicationDX::Run()
{
	while (true)
	{
		deltaTime = fpsTimer.frameTimeElapsed() * fpsTimer.milliToSec;
		fpsTimer.resetClock();
		if (!ProcessInputs())
		{

			return;
		}
		const float currentTime = fpsTimer.getCurrentTick();
		Update(deltaTime, currentTime);
		engine.render();
	}
}


bool ApplicationDX::ProcessInputs()
{
	float xPos{ lastMousePos.x };
	float yPos{ lastMousePos.y };

	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (!ImGui::GetIO().WantCaptureMouse && (pitchYawRotation || rayCasted))
			{
				xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
				yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			}
			break;
		case WM_LBUTTONDOWN:
			if (!ImGui::GetIO().WantCaptureMouse)
			{
				pitchYawRotation = true;
				lastMousePos.x = xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
				lastMousePos.y = yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			}
			break;
		case WM_LBUTTONUP:
			pitchYawRotation = false;
			break;
		case WM_RBUTTONDOWN:
			rayCasted = true;
			lastMousePos.x = xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
			lastMousePos.y = yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			break;
		case WM_RBUTTONUP:
			rayCasted = objectCaptured = false;

			break;
		case WM_MOUSEWHEEL:
			if (GET_WHEEL_DELTA_WPARAM(msg.wParam) > 0)
			{
				cameraSpeed *= 1.05;
			}
			else
			{
				cameraSpeed /= 1.05;
			}
			break;
		case WM_KEYDOWN:
			switch (msg.wParam)
			{
			case VK_OEM_PLUS:
				exposureState[(int)ChangeState::INCREASE] = true;
				break;
			case VK_OEM_MINUS:
				exposureState[(int)ChangeState::DECREASE] = true;
				break;
			case VK_NUMPAD1:
			case '1':
				engine.getRenderer().changeRenderState(engine::DX::SamplerState::BindSlot::POINT_WRAP);
				break;
			case VK_NUMPAD2:
			case '2':
				engine.getRenderer().changeRenderState(engine::DX::SamplerState::BindSlot::LINEAR_WRAP);
				break;
			case VK_NUMPAD3:
			case '3':
				engine.getRenderer().changeRenderState(engine::DX::SamplerState::BindSlot::ANISOTROPIC_WRAP);
				break;
			case 0x57: // W
				cameraMovingDirections[MoveDirection::FORWARD] = true;
				break;
			case 0x41: // A
				cameraMovingDirections[MoveDirection::LEFT] = true;
				break;
			case 0x53: // S
				cameraMovingDirections[MoveDirection::BACK] = true;
				break;
			case 0x44: // D
				cameraMovingDirections[MoveDirection::RIGHT] = true;
				break;
			case 0x4E: // N
				nomalVisualized = !nomalVisualized;
				engine.normalVisualization(nomalVisualized);
				break;
			case 0x45: // E
				wireframeMode = !wireframeMode;
				engine.getRenderer().changeWireframe(wireframeMode);
				break;
			case 0x46: // F
				flashLightAttached = !flashLightAttached;
				if (cameraFlashLight != -1)
				{
					LightSystem::getInstance().getFlashLight(cameraFlashLight).attachedToCamera = flashLightAttached;
				}
				else
				{
					std::cerr << "Flashlight is not attached!\n";
				}
				break;
			case 0x4D: // M
			{

				std::vector<size_t> meshIndices(spawnModel.get()->getMeshesCount());
				for (size_t i = 0; i < meshIndices.size(); i++)
				{
					meshIndices[i] = i;
				}

				const engine::DX::float3 cameraPos = engine.getCamera().position();
				const engine::DX::float3 objPos = cameraPos + engine.getCamera().forward() * engine::DX::DissolutionInstances::SPAWN_DISTANCE;

				constexpr engine::DX::float3 up = { 0, 1, 0 };
				engine::DX::float3 direction = objPos - cameraPos;
				direction.Normalize();
				engine::DX::float3 right = up.Cross(direction);
				right.Normalize();
				const engine::DX::float3 newUp = direction.Cross(right);

				engine::DX::float4x4 instance = engine::DX::float4x4{
					{right.x, newUp.x, direction.x, objPos.x},
					{right.y, newUp.y, direction.y, objPos.y},
					{right.z, newUp.z, direction.z, objPos.z},
					{0,       0,       0,          1}
				};


				//rotation and scale matrices for more flexible model creation
				//constexpr float theta = DirectX::XMConvertToRadians(90.0f); // -90 degrees converted to radians for clockwise rotation
				//auto rotationMatrix = DirectX::SimpleMath::Matrix::CreateRotationX(theta);
				//engine::DX::float4x4 scale = engine::DX::float4x4{
				//	{10, 0, 0, 0},
				//	{0, 10, 0, 0},
				//	{0, 0, 10, 0},
				//	{0,       0,       0,          1}
				//};
				//instance = instance * scale * rotationMatrix;

				auto modelInstanceIDs = engine.addDissolutionInstances(dissolutionInstancesID, spawnModel, meshIndices, dissolutionMaterials[DISSOLUTION_SKIN], { instance });

				const float currentTime = fpsTimer.getCurrentTick();
				for (size_t i = 0; i < modelInstanceIDs.size(); i++)
				{
					DissolutionAnimation animation;
					animation.initTime = currentTime;
					animation.maxTime = dissolutionEffectDuration;
					animation.modelInstanceID = modelInstanceIDs[i];

					dissolutionAnimations.push_back(animation);
				}
			}
			break;
			case VK_SPACE:
				cameraMovingDirections[MoveDirection::UP] = true;
				break;
			case VK_CONTROL:
				cameraMovingDirections[MoveDirection::DOWN] = true;
				break;
			case VK_SHIFT:
				if (!increasedSpeed)
				{
					cameraSpeed *= 5;
					increasedSpeed = true;
				}
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.wParam)
			{
			case VK_OEM_PLUS:
				exposureState[(int)ChangeState::INCREASE] = false;
				break;
			case VK_OEM_MINUS:
				exposureState[(int)ChangeState::DECREASE] = false;
				break;
			case 0x57: // W
				cameraMovingDirections[MoveDirection::FORWARD] = false;
				break;
			case 0x41: // A
				cameraMovingDirections[MoveDirection::LEFT] = false;
				break;
			case 0x53: // S
				cameraMovingDirections[MoveDirection::BACK] = false;
				break;
			case 0x44: // D
				cameraMovingDirections[MoveDirection::RIGHT] = false;
				break;
			case VK_SPACE:
				cameraMovingDirections[MoveDirection::UP] = false;
				break;
			case VK_CONTROL:
				cameraMovingDirections[MoveDirection::DOWN] = false;
				break;
			case VK_SHIFT:
				if (increasedSpeed)
				{
					cameraSpeed /= 5;
					increasedSpeed = false;
				}
				break;
			}
			break;
		case WM_QUIT:
			return false;
			break;
		}



		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}


	AddCameraDirection();
	if (!objectCaptured && rayCasted)
	{
		objectCaptured = engine.castRay(xPos, yPos);
#ifdef INTERSECTION_TEST
		engine.castRay(engine.getWindow().GetWidth() / 2, engine.getWindow().GetHeight() / 2);
		//cast the grid of rays
		engine.castRay();
#endif // INTERSECTION_TEST
}
	if (objectCaptured)
	{
		MoveObject(xPos, yPos);
	}
	if (pitchYawRotation)
	{
		RotateCamera(xPos, yPos);
	}
	MoveCamera();
	HandleExposure();
	lastMousePos = { xPos, yPos };
	cameraDirection = engine::DX::float3();
	return true;
}

void ApplicationDX::Update(float deltaTime, float currentTime)
{
	UpdateParticleAnimation(deltaTime);
	UpdateDissolutionAnimation(currentTime);
}

void ApplicationDX::UpdateParticleAnimation(float deltaTime)
{
	auto& particleSystem = engine.getParticleSystem();
	particleSystem.update(deltaTime);
}

void ApplicationDX::UpdateDissolutionAnimation(float currentTime)
{
	std::vector<engine::DX::TransformSystem::ID> newOpaqueTransformIDs;
	removeFinishedDissolutionAnimations(currentTime, newOpaqueTransformIDs);

	std::vector<size_t> meshIndices(spawnModel.get()->getMeshesCount());
	for (size_t i = 0; i < meshIndices.size(); i++)
	{
		meshIndices[i] = i;
	}
	if (!newOpaqueTransformIDs.empty())
	{
		engine.addOpaqueInstances(iblOpaqueInstance, spawnModel, meshIndices, opaqueMaterials[DEFAULT_SKIN], newOpaqueTransformIDs);
		std::cout << "added opaques\n";
	}

}

void ApplicationDX::removeFinishedDissolutionAnimations(const float currentTime, std::vector<engine::DX::TransformSystem::ID>& newOpaqueTransformIDs)
{
	dissolutionAnimations.erase(std::remove_if(dissolutionAnimations.begin(), dissolutionAnimations.end(),
		[&](const auto& dissolutionAnimation) {
			if (isAnimationFinished(currentTime, dissolutionAnimation)) {
				handleFinishedAnimation(dissolutionAnimation, newOpaqueTransformIDs);
				return true;
			}
			else {
				updateAnimationTime(currentTime, dissolutionAnimation);
				return false;
			}
		}),
		dissolutionAnimations.end());
}

bool ApplicationDX::isAnimationFinished(const float currentTime, const DissolutionAnimation& dissolutionAnimation)
{
	return dissolutionAnimation.maxTime - (currentTime - dissolutionAnimation.initTime) <= 0;
}

void ApplicationDX::handleFinishedAnimation(const DissolutionAnimation& dissolutionAnimation, std::vector<engine::DX::TransformSystem::ID>& newOpaqueTransformIDs)
{
	engine.removeDissolutionInstance(dissolutionInstancesID, dissolutionAnimation.modelInstanceID);
	std::cout << "deleted dissolution\n";
	newOpaqueTransformIDs.push_back(dissolutionAnimation.modelInstanceID.transformID);
}

void ApplicationDX::updateAnimationTime(const float currentTime, const DissolutionAnimation& dissolutionAnimation)
{
	float normalizedTime = (currentTime - dissolutionAnimation.initTime) / dissolutionAnimation.maxTime;
	//std::cout << normalizedTime << std::endl;

	auto instance = engine.getDissolutionInstance(dissolutionInstancesID, dissolutionAnimation.modelInstanceID);
	instance->time = normalizedTime;
}


void ApplicationDX::AddCameraDirection()
{
	if (cameraMovingDirections[MoveDirection::FORWARD])
		cameraDirection += engine.getCamera().forward();
	if (cameraMovingDirections[MoveDirection::BACK])
		cameraDirection -= engine.getCamera().forward();
	if (cameraMovingDirections[MoveDirection::LEFT])
		cameraDirection -= engine.getCamera().right();
	if (cameraMovingDirections[MoveDirection::RIGHT])
		cameraDirection += engine.getCamera().right();
	if (cameraMovingDirections[MoveDirection::UP])
		cameraDirection += engine.getCamera().up();
	if (cameraMovingDirections[MoveDirection::DOWN])
		cameraDirection -= engine.getCamera().up();
}



bool ApplicationDX::MoveCamera()
{

	bool isCameraMove{ cameraDirection != engine::DX::float3() };
	if (isCameraMove)
	{
		engine::DX::float3 moveOffset{ cameraDirection * cameraSpeed * deltaTime };
		engine.getCamera().addWorldOffset(moveOffset);
	}
	return isCameraMove;
}

void ApplicationDX::MoveObject(float xPos, float yPos)
{

	bool isObjMove{ cameraDirection != engine::DX::float3() };
	if (isObjMove)
	{
		engine::DX::float3 moveOffset{ cameraDirection * cameraSpeed * deltaTime };
		engine.moveCapturedObject(moveOffset);
	}
	else if (objectCaptured)
	{
		engine.dragCapturedObject(xPos - lastMousePos.x, lastMousePos.y - yPos);
	}

}

void ApplicationDX::RotateCamera(float xPos, float yPos)
{
	engine::DX::float2 offset{ lastMousePos.x - xPos, lastMousePos.y - yPos };
	if (std::abs(offset.x) > FLT_EPSILON || std::abs(offset.y) > FLT_EPSILON)
	{
		float pitch = offset.y * cameraRotationSpeed;
		cameraPitch += pitch;
		if (!(cameraPitch < cameraMaxPitch))
		{
			cameraPitch = cameraMaxPitch;
			pitch = 0;
		}
		else if (!(cameraPitch > cameraMinPitch))
		{
			cameraPitch = cameraMinPitch;
			pitch = 0;
		}
		engine.getCamera().addWorldAngles({ pitch , offset.x * cameraRotationSpeed, 0 });
	}


}

void ApplicationDX::HandleExposure()
{
	if (exposureState[(int)ChangeState::INCREASE] || exposureState[(int)ChangeState::DECREASE])
	{
		auto& postProcess = engine.getRenderer().getPostProcess();
		float exposure = postProcess.getEV100();

		if (exposureState[(int)ChangeState::INCREASE])
		{
			exposure += deltaExposure * deltaTime;
		}
		if (exposureState[(int)ChangeState::DECREASE])
		{
			exposure -= deltaExposure * deltaTime;
		}

		postProcess.setEV100(exposure);
	}
}

std::vector<engine::DX::float4x4> ApplicationDX::generateRandomInstances(size_t numInstances)
{

	std::vector<engine::DX::float4x4> instances(numInstances);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distNeg(-50.f, -1.f);
	std::uniform_real_distribution<float> distPos(1.f, 50.f);



	for (size_t j = 0; j < numInstances; j++)
	{
		float x = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);
		float y = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);
		float z = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);

		instances.at(j) = engine::DX::float4x4{
			{{1,0,0,x},
			{0,1,0,y},
			{0,0,1,z},
			{0,0,0,1},}
		};

	}

	return instances;
}
