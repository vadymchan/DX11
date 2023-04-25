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
	const int windowHeight{ 800 };
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


	const std::wstring skyboxTexture			{ engine::DX::textureDirectory / L"skybox/hdr/night_street.dds" };
	//const std::wstring skyboxTexture			{ engine::DX::textureDirectory / L"skybox/hdr/grass_field.dds" };

	const std::wstring flashLight	{ engine::DX::textureDirectory / L"flashLight/dragonslake.dds" };

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

	engine::DX::D3D::Init();


	engine.initWindow("DirectX 11 Model", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);

	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = wireframeMode ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;

	engine::DX::Skybox skybox;
	skybox.initSkybox(skyboxTexture);

	engine.initRenderer(rasterizerDesc, std::make_shared<engine::DX::Skybox>(skybox));
	//shaders & input layout
	// -------------------------------------------------------------------------------------------------
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(2), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> pointLightInputElementDesc
	{
		// per-vertex data
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, engine::DX::MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},
		// per-instance data
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"EMISSION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, engine::DX::INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_INSTANCE_DATA, 1},
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
	ShaderManager::getInstance().addVertexShader(normalVertexShaderFileName, inputElementDesc);
	ShaderManager::getInstance().addVertexShader(colorVertexShaderFileName, inputElementDesc);
	ShaderManager::getInstance().addVertexShader(blinnPhongVertexShaderFileName, inputElementDesc);
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

	//textures & sampler 
	//---------------------------------------------------------------------------------------------------

	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// opaque instance
	//---------------------------------------------------------------------------------------------------

	uint32_t normalOpaqueInstance = engine.createOpaqueInstance({ {blinnPhongVertexShaderFileName, L"", L"", L"",blinnPhongPixelShaderFileName} /*{colorVertexShaderFileName, L"", L"", L"",colorPixelShaderFileName}*/, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	uint32_t sphereOpaqueInstance = engine.createOpaqueInstance({ {pointLightVertexShaderFileName, L"", L"", L"",pointLightPixelShaderFileName} /*{colorVertexShaderFileName, L"", L"", L"",colorPixelShaderFileName}*/, {normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName,normalPixelShaderFileName} });
	uint32_t hologramOpaqueInstance = engine.createOpaqueInstance({ {hologramVertexShaderFileName, L"", L"", /*hologramHullShaderFileName, hologramDomainShaderFileName,*/ hologramGeometryShaderFileName, hologramPixelShaderFileName}, /*{normalVertexShaderFileName, L"", L"", normalGeometryShaderFileName, normalPixelShaderFileName},*/ });


	
	//light
	//---------------------------------------------------------------------------------------------------
	
	//point light 
	std::vector<engine::DX::float4> pointLightEmissions{ {0,1,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,1,0} };
	std::vector<float> pointLightIntensities{ 10, 10, 10, 10 };
	std::vector<engine::DX::float3> pointLightColors{ {1,0,1}, {0,0,1}, {1,0,0}, {1,1,0}, };
	std::vector<Attenuation> pointLightAttenuations{ { 1.0, 0.045, 0.0075 } };


	// models
	//---------------------------------------------------------------------------------------------------
	std::shared_ptr<Model> cube = ModelManager::getInstance().getModel(ModelManager::cubeTag);
	std::shared_ptr<Model> sphere = ModelManager::getInstance().getModel(ModelManager::sphereTag);
	std::shared_ptr<Model> flatSphere = ModelManager::getInstance().getModel(ModelManager::flatSphereTag);
	std::shared_ptr<Model> samurai = ModelManager::getInstance().getModel("Samurai/Samurai.fbx");
	std::shared_ptr<Model> eastTower = ModelManager::getInstance().getModel("EastTower/EastTower.fbx");
	std::shared_ptr<Model> knight = ModelManager::getInstance().getModel("Knight/Knight.fbx");
	std::shared_ptr<Model> knightHorse = ModelManager::getInstance().getModel("KnightHorse/KnightHorse.fbx");




	std::vector<engine::DX::float4x4> samuraiInstance
	{

			 engine::DX::float4x4 {
				{1,0,0,-2},
				{0,1,0,0},
				{0,0,1,0},
				{0,0,0,1},
			} ,
	};


	std::vector<engine::DX::float4x4> eastTowerInstances
	{

			engine::DX::float4x4
			{{{1,0,0,2},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}}
		,
	};

	std::vector<engine::DX::float4x4> knightInstances
	{


			engine::DX::float4x4
			{{{1,0,0,-4},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}}
		,
	};

	std::vector<engine::DX::float4x4> knightHorseInstances
	{

			engine::DX::float4x4
			{{{1,0,0,-6},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}},
	};

	std::vector<engine::DX::float4x4> spherePointLightInstances
	{

		engine::DX::float4x4
			{{{pointLightIntensities[0] * 0.05f,0,0,-10},
			{0,pointLightIntensities[0] * 0.05f,0,10},
			{0,0,pointLightIntensities[0] * 0.05f,0},
			{0,0,0,1},}},
		engine::DX::float4x4
			{{{pointLightIntensities[0] * 0.05f,0,0,10},
			{0,pointLightIntensities[0] * 0.05f,0,10},
			{0,0,pointLightIntensities[0] * 0.05f,0},
			{0,0,0,1},}},
		engine::DX::float4x4
			{{{pointLightIntensities[0] * 0.05f,0,0,-10},
			{0,pointLightIntensities[0] * 0.05f,0,-10},
			{0,0,pointLightIntensities[0] * 0.05f,0},
			{0,0,0,1},}},
		engine::DX::float4x4
			{{{pointLightIntensities[0] * 0.05f,0,0,10},
			{0,pointLightIntensities[0] * 0.05f,0,-10},
			{0,0,pointLightIntensities[0] * 0.05f,0},
			{0,0,0,1},}},
	};


	std::vector<size_t> cubeMeshIndices;
	for (size_t i = 0; i < cube.get()->getMeshesCount(); ++i)
	{
		cubeMeshIndices.emplace_back(i);
	}



	std::vector<engine::DX::float4x4> obsidianCubeInstance{};

	std::vector<engine::DX::float4x4> diamondInstance{};

	for (size_t i = 0; i < 16; i++)
	{
		std::vector<engine::DX::float4x4>* first;
		std::vector<engine::DX::float4x4>* second;
		if (i & 1)
		{
			first = &obsidianCubeInstance;
			second = &diamondInstance;
		}
		else
		{
			first = &diamondInstance;
			second = &obsidianCubeInstance;
		}

		for (size_t j = 0; j < 16; j++)
		{

			float x = ((float)j - 0) * (8 + 8) / (15 - 0) - 8;
			float y = ((float)i - 0) * (-8 - 8) / (15 - 0) + 8;
			auto instance =
				engine::DX::float4x4{
				   {1,0,0,x },
				   {0,1,0,y },
				   {0,0,1,5},
				   {0,0,0,1},
			};

			if (j & 1)
			{
				first->push_back(instance);
			}
			else
			{
				second->push_back(instance);
			}
		}
	}

	std::shared_ptr<Material> diamond = std::make_shared<Material>(Material{ L"diamond" });
	std::shared_ptr<Material> obsidian = std::make_shared<Material>(Material{ L"obsidian" });
	std::shared_ptr<Material> noMaterial = std::make_shared<Material>(Material{ L"" });
	std::shared_ptr<Material> defaultSkin = std::make_shared<Material>(Material{ L"default" });

	engine.addInstancedModel(normalOpaqueInstance, cube, cubeMeshIndices, diamond, diamondInstance);
	engine.addInstancedModel(normalOpaqueInstance, cube, cubeMeshIndices, obsidian, obsidianCubeInstance);


	std::vector<size_t> samuraiMeshIndices;
	for (size_t i = 0; i < samurai.get()->getMeshesCount(); ++i)
	{
		samuraiMeshIndices.emplace_back(i);
	}

	engine.addInstancedModel(normalOpaqueInstance, samurai, samuraiMeshIndices, defaultSkin, samuraiInstance);


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
	engine.addInstancedModel(normalOpaqueInstance, eastTower, eastTowerMeshIndices, defaultSkin, eastTowerInstances);


	std::vector<size_t> knightMeshIndices;
	for (size_t i = 0; i < knight.get()->getMeshesCount(); ++i)
	{
		knightMeshIndices.emplace_back(i);
	}
	engine.addInstancedModel(normalOpaqueInstance, knight, knightMeshIndices, defaultSkin, knightInstances);

	std::vector<size_t> knightHorseMeshIndices;
	for (size_t i = 0; i < knightHorse.get()->getMeshesCount(); ++i)
	{
		knightHorseMeshIndices.emplace_back(i);
	}
	engine.addInstancedModel(normalOpaqueInstance, knightHorse, knightHorseMeshIndices, defaultSkin, knightHorseInstances);

	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const engine::DX::float3& position{ 0, 0, -5 };
	const engine::DX::float3& direction{ 0, 0,  1 };
	const engine::DX::float3& cameraUp{ 0,1,0 };

	float fov{ 90 };
	float aspect{ (float)windowWidth / windowHeight };
	float zNear{ 0.01 };
	float zFar{ 100 };

	engine.initCamera(position, direction, cameraUp, fov, aspect, zNear, zFar);

	// light 
	//---------------------------------------------------------------------------------------------------



	LightSystem& lightSystem = LightSystem::getInstance();

	lightSystem.addDirectionalLight({ 1,1, 1 }, 0.5, { 0, -0.5, -0.5 ,  0 });

	cameraFlashLight = lightSystem.addFlashLight(
		{ 1,1,1 },
		10,
		DirectX::XMConvertToRadians(20.0f),
		DirectX::XMConvertToRadians(30.0f),
		pointLightAttenuations[0],
		flashLight
	);

	

	lightSystem.addSpotLight(
		{ 0, 0, 17 },
		{ 0, 0,  -1 },
		{ 0.69,0,0 },
		10,
		DirectX::XMConvertToRadians(30.0f),
		DirectX::XMConvertToRadians(30.0f),
		pointLightAttenuations[0]
	);

	for (int i = 0; i < pointLightInstanceIDs.size(); ++i)
	{
		lightSystem.addPointLight(
			pointLightInstanceIDs[i],
			pointLightColors[i],
			pointLightIntensities[i],
			pointLightAttenuations[0]);

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
				engine.getRenderer().changeRenderState(engine::DX::SampleState::BindSlot::POINT_WRAP);
				break;
			case VK_NUMPAD2:
			case '2':
				engine.getRenderer().changeRenderState(engine::DX::SampleState::BindSlot::LINEAR_WRAP);
				break;
			case VK_NUMPAD3:
			case '3':
				engine.getRenderer().changeRenderState(engine::DX::SampleState::BindSlot::ANISOTROPIC_WRAP);
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
				LightSystem::getInstance().getFlashLight(cameraFlashLight).attachedToCamera = flashLightAttached;
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

		instances.at(j) = engine::DX::float4x4 {
			{{1,0,0,x},
			{0,1,0,y},
			{0,0,1,z},
			{0,0,0,1},}
		};

	}

	return instances;
}
