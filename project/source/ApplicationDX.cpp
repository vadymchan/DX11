#include "ApplicationDX.h"
#include <random>

using Instance = engine::DX::OpaqueInstances::Instance;
using Material = engine::DX::OpaqueInstances::Material;
using Attenuation = engine::DX::LightSystem::Attenuation;
using engine::DX::Model;
using engine::DX::Mesh;
using engine::DX::MeshSystem;
using engine::DX::ModelManager;
using engine::DX::ShaderManager;
using engine::DX::TextureManager;
using engine::DX::LightSystem;

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

	const std::wstring skyboxNightStreet{ engine::DX::textureDirectory / L"skybox/hdr" / "night_street.dds" };
	const std::wstring skyboxGrassField{ engine::DX::textureDirectory / L"skybox/hdr" / "grass_field.dds" };

	const std::wstring flashLight{ engine::DX::textureDirectory / L"flashLight/dragonslake.dds" };

	cameraSpeed = 2.f;
	cameraRotationSpeed = 0.005f;
	cameraMaxPitch = DirectX::XMConvertToRadians(89.0f);
	cameraMinPitch = DirectX::XMConvertToRadians(-89.0f);
	wireframeMode = false;
	deltaExposure = 1.f;
	flashLightAttached = true;
#ifdef _DEBUG
	engine::general::initConsole();
#endif

	// compute integral of cosine over the hemisphere using Fibonacci hemisphere point distribution
	// --------------------------------------------------------------------------------------------
	std::cout << "Integral over the hemisphere: " << engine::hemisphereIntegral(1000000) << std::endl;
	// --------------------------------------------------------------------------------------------

	engine::DX::D3D::Init();

	D3D11_TEXTURE2D_DESC textureDesc1{};
	textureDesc1.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc1.Usage = D3D11_USAGE_DEFAULT;
	textureDesc1.CPUAccessFlags = 0;
	textureDesc1.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	//engine::DX::TextureManager::getInstance().addTexture2D(skyboxGrassField, 0, textureDesc1);
	//auto texture = engine::DX::TextureManager::getInstance().getTexture2D(skyboxGrassField);

	//engine::DX::ReflectionCapture reflectionCapture;

	D3D11_TEXTURE2D_DESC textureDesc2{};
	textureDesc2.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc2.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc2.CPUAccessFlags = 0;


	D3D11_TEXTURE2D_DESC textureDesc3{};
	textureDesc3.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc3.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc3.CPUAccessFlags = 0;
	textureDesc3.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;



	engine.initWindow("DirectX 11 Model", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = wireframeMode ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;

	engine::DX::Skybox skybox;
	//skybox.initSkybox(engine::DX::ReflectionCapture::PRECOMPUTE_IRRADIANCE_PATH / L"grass_field_Irradiance.dds");
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

	//vertex shader
	ShaderManager::getInstance().addVertexShader(normalVertexShaderFileName, inputElementDescNormal);
	ShaderManager::getInstance().addVertexShader(colorVertexShaderFileName, inputElementDesc);
	ShaderManager::getInstance().addVertexShader(blinnPhongVertexShaderFileName, inputElementDescNormal);
	ShaderManager::getInstance().addVertexShader(pbrVertexShaderFileName, pbrInputElementDesc);
	ShaderManager::getInstance().addVertexShader(iblVertexShaderFileName, pbrInputElementDesc);
	ShaderManager::getInstance().addVertexShader(hologramVertexShaderFileName, hologramInputElementDesc);
	ShaderManager::getInstance().addVertexShader(pointLightVertexShaderFileName, pointLightInputElementDesc);
	//hull shader
	ShaderManager::getInstance().addHullShader(hologramHullShaderFileName);
	//domain shader
	ShaderManager::getInstance().addDomainShader(hologramDomainShaderFileName);
	//geometry shader
	ShaderManager::getInstance().addGeometryShader(normalGeometryShaderFileName);
	ShaderManager::getInstance().addGeometryShader(hologramGeometryShaderFileName);
	//pixel shader
	ShaderManager::getInstance().addPixelShader(normalPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(colorPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(hologramPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(blinnPhongPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(pointLightPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(pbrPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(iblPixelShaderFileName);

	//textures & sampler 
	//---------------------------------------------------------------------------------------------------

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// opaque instance
	//---------------------------------------------------------------------------------------------------

	uint32_t normalOpaqueInstance = engine.createOpaqueInstance({ {blinnPhongVertexShaderFileName, L"", L"", L"",blinnPhongPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	uint32_t pbrOpaqueInstance = engine.createOpaqueInstance({ {pbrVertexShaderFileName, L"", L"", L"",pbrPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	uint32_t iblOpaqueInstance = engine.createOpaqueInstance({ {iblVertexShaderFileName, L"", L"", L"",iblPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	uint32_t sphereOpaqueInstance = engine.createOpaqueInstance({ {pointLightVertexShaderFileName, L"", L"", L"",pointLightPixelShaderFileName}, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	uint32_t hologramOpaqueInstance = engine.createOpaqueInstance({ {hologramVertexShaderFileName, L"", L"", /*hologramHullShaderFileName, hologramDomainShaderFileName,*/ hologramGeometryShaderFileName, hologramPixelShaderFileName}, /*{normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName, normalPixelShaderFileName},*/ });



	//light
	//---------------------------------------------------------------------------------------------------

	//point light 
	std::vector<engine::DX::float4> pointLightEmissions{ {1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {1,1,1,0}, };
	std::vector<float> pointLightIntensities{ 100, 100, 100, 100 };
	std::vector<engine::DX::float3> pointLightColors{ {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {1, 1, 1}, };
	std::vector<Attenuation> pointLightAttenuations{ { 1.0, 0.045, 0.0075 }, { 1.0, 0.045, 0.0075 }, { 1.0, 0.045, 0.0075 }, { 1.0, 0.045, 0.0075 }, };
	std::vector<float> pointLightRadiuses{ 0.5, 0.5, 0.5, 0.5, };


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




	std::vector<engine::DX::float4x4> samuraiInstance
	{
			 engine::DX::float4x4 {
				{1,0,0,2},
				{0,1,0,0},
				{0,0,1,2},
				{0,0,0,1},
			} ,
	};


	std::vector<engine::DX::float4x4> eastTowerInstances
	{
			engine::DX::float4x4
			{{{1,0,0,5},
			{0,1,0,0},
			{0,0,1,2},
			{0,0,0,1},}},
	};

	std::vector<engine::DX::float4x4> knightInstances
	{
			engine::DX::float4x4
			{{{1,0,0,-2},
			{0,1,0,0},
			{0,0,1,2},
			{0,0,0,1},}}
		,
	};

	std::vector<engine::DX::float4x4> knightHorseInstances
	{

			engine::DX::float4x4
			{{{1,0,0,-4},
			{0,1,0,0},
			{0,0,1,2},
			{0,0,0,1},}},
	};

	std::vector<engine::DX::float4x4> spherePointLightInstances
	{


		engine::DX::float4x4
			{{{pointLightRadiuses[0],0,0,5},
			{0,pointLightRadiuses[0],0,5},
			{0,0,pointLightRadiuses[0],-3},
			{0,0,0,1},}},
		engine::DX::float4x4
			{{{pointLightRadiuses[1],0,0,-5},
			{0,pointLightRadiuses[1],0,5},
			{0,0,pointLightRadiuses[1],-4},
			{0,0,0,1},}},
		engine::DX::float4x4
			{{{pointLightRadiuses[2],0,0,-5},
			{0,pointLightRadiuses[2],0,0},
			{0,0,pointLightRadiuses[2],-2},
			{0,0,0,1},}},
		engine::DX::float4x4
			{{{pointLightRadiuses[3],0,0,5},
			{0,pointLightRadiuses[3],0,0},
			{0,0,pointLightRadiuses[3],-4},
			{0,0,0,1},}},
	};

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



	std::shared_ptr<Material> diamond = std::make_shared<Material>(Material{ L"diamond" });
	std::shared_ptr<Material> obsidian = std::make_shared<Material>(Material{ L"obsidian" });
	std::shared_ptr<Material> grass = std::make_shared<Material>(Material{ L"Grass" });
	std::shared_ptr<Material> peacock = std::make_shared<Material>(Material{ L"Peacock" });
	std::shared_ptr<Material> vines = std::make_shared<Material>(Material{ L"Vines" });
	std::shared_ptr<Material> normalDebug = std::make_shared<Material>(Material{ L"normalDebug" });
	std::shared_ptr<Material> mirror = std::make_shared<Material>(Material{ L"Mirror" });
	std::shared_ptr<Material> shinyConductor = std::make_shared<Material>(Material{ L"Shiny Conductor" });
	std::shared_ptr<Material> roughDielectric = std::make_shared<Material>(Material{ L"Rough Dielectric" });
	std::shared_ptr<Material> noMaterial = std::make_shared<Material>(Material{ L"" });
	std::shared_ptr<Material> defaultSkin = std::make_shared<Material>(Material{ L"default" });

	engine.addInstancedModel(iblOpaqueInstance, sphere, cubeMeshIndices, mirror, mirrorSphere);
	engine.addInstancedModel(iblOpaqueInstance, sphere, cubeMeshIndices, shinyConductor, shinyConductorSphere);
	engine.addInstancedModel(iblOpaqueInstance, sphere, cubeMeshIndices, roughDielectric, roughDielectricSphere);


	engine.addInstancedModel(iblOpaqueInstance, floor, { 0 }, mirror, floorInstances);

	std::vector<size_t> samuraiMeshIndices;
	for (size_t i = 0; i < samurai.get()->getMeshesCount(); ++i)
	{
		samuraiMeshIndices.emplace_back(i);
	}

	engine.addInstancedModel(iblOpaqueInstance, samurai, samuraiMeshIndices, defaultSkin, samuraiInstance);


	std::vector<size_t> pointLightMeshIndeices;
	for (size_t i = 0; i < sphere.get()->getMeshesCount(); ++i)
	{
		pointLightMeshIndeices.emplace_back(i);
	}

	std::vector<engine::DX::TransformSystem::ID> pointLightInstanceIDs =
		engine.addInstancedModel(sphereOpaqueInstance, sphere, pointLightMeshIndeices, noMaterial, spherePointLightInstances, pointLightEmissions);


	std::vector<size_t> eastTowerMeshIndices;
	for (size_t i = 0; i < eastTower.get()->getMeshesCount(); ++i)
	{
		eastTowerMeshIndices.emplace_back(i);
	}
	engine.addInstancedModel(iblOpaqueInstance, eastTower, eastTowerMeshIndices, defaultSkin, eastTowerInstances);


	std::vector<size_t> knightMeshIndices;
	for (size_t i = 0; i < knight.get()->getMeshesCount(); ++i)
	{
		knightMeshIndices.emplace_back(i);
	}
	engine.addInstancedModel(iblOpaqueInstance, knight, knightMeshIndices, defaultSkin, knightInstances);

	std::vector<size_t> knightHorseMeshIndices;
	for (size_t i = 0; i < knightHorse.get()->getMeshesCount(); ++i)
	{
		knightHorseMeshIndices.emplace_back(i);
	}
	engine.addInstancedModel(iblOpaqueInstance, knightHorse, knightHorseMeshIndices, defaultSkin, knightHorseInstances);

	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const engine::DX::float3 position{ 0, 2, -2 };
	engine::DX::float3 direction{ 0, 0, 1 };
	direction.Normalize();
	const engine::DX::float3 worldUp{ 0, 1, 0 };


	float fov{ 60 };
	float aspect{ (float)windowWidth / windowHeight };
	float zNear{ 0.01 };
	float zFar{ 100 };

	engine.initCamera(position, direction, worldUp, fov, aspect, zNear, zFar);

	// light 
	//---------------------------------------------------------------------------------------------------



	LightSystem& lightSystem = LightSystem::getInstance();

	//lightSystem.addDirectionalLight({ 1, 1, 1 }, 10, { 0, -0.5, -0.5 }, 1);

	//cameraFlashLight = lightSystem.addFlashLight(
	//	{ 1,1,1 },
	//	1000,
	//	DirectX::XMConvertToRadians(10.0f),
	//	DirectX::XMConvertToRadians(15.0f),
	//	0.1,
	//	pointLightAttenuations[0],
	//	flashLight
	//);

	//lightSystem.addSpotLight(
	//	{ 0, 3, 5 },
	//	{ 0, -1, 0 },
	//	{ 0.69,0,0 },
	//	10,
	//	DirectX::XMConvertToRadians(30.0f),
	//	DirectX::XMConvertToRadians(30.0f),
	//	1,
	//	pointLightAttenuations[0]
	//);

	for (int i = 0; i < pointLightInstanceIDs.size(); ++i)
	{
		lightSystem.addPointLight(
			pointLightInstanceIDs[i],
			pointLightColors[i],
			pointLightIntensities[i],
			pointLightRadiuses[i],
			pointLightAttenuations[i]);

	};


	return;
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
