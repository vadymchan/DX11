#include "ApplicationDX.h"
#include <glm/trigonometric.hpp>
#include <random>



void ApplicationDX::Init(const HINSTANCE& appHandle, int windowShowParams)
{
	const int windowWidth{ 1000 };
	const int windowHeight{ 800 };
	const int windowStartX{ 0 };
	const int windowStartY{ 0 };
	const std::wstring normalVertexShaderFileName{ L"vertexShader.hlsl" };
	const std::wstring normalPixelShaderFileName{ L"pixelShader.hlsl" };
	const std::wstring hologramVertexShaderFileName{ L"hologramVertexShader.hlsl" };
	const std::wstring hologramPixelShaderFileName{ L"hologramPixelShader.hlsl" };
	cameraSpeed = 2.f;
#ifdef _DEBUG
	engine::general::initConsole();
#endif

	engine::DX::D3D::Init();


	engine.initWindow(L"DirectX 11 Model", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);

	//shaders & input layout
	// -------------------------------------------------------------------------------------------------
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},

		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	ShaderManager::getInstance().addVertexShader(normalVertexShaderFileName, inputElementDesc);
	ShaderManager::getInstance().addVertexShader(hologramVertexShaderFileName, inputElementDesc);
	ShaderManager::getInstance().addPixelShader(normalPixelShaderFileName);
	ShaderManager::getInstance().addPixelShader(hologramPixelShaderFileName);


	// opaque instance
	//---------------------------------------------------------------------------------------------------

	uint32_t normalOpaqueInstance = engine.createOpaqueInstance(normalVertexShaderFileName, normalPixelShaderFileName);
	uint32_t hologramOpaqueInstance = engine.createOpaqueInstance(hologramVertexShaderFileName, hologramPixelShaderFileName);

	// models
	//---------------------------------------------------------------------------------------------------
	std::shared_ptr<Model> cube = ModelManager::getInstance().loadCube();
	std::shared_ptr<Model> samurai = ModelManager::getInstance().loadAssimp("Samurai/Samurai.fbx");
	std::shared_ptr<Model> eastTower = ModelManager::getInstance().loadAssimp("EastTower/EastTower.fbx");
	std::shared_ptr<Model> knight = ModelManager::getInstance().loadAssimp("Knight/Knight.fbx");
	std::shared_ptr<Model> knightHorse = ModelManager::getInstance().loadAssimp("KnightHorse/KnightHorse.fbx");


	std::vector<std::shared_ptr<Material>> materials(8);
	for (auto& material : materials)
	{
		material = std::make_shared<Material>();
	}
	materials.at(0)->color = { 1,0,0,0};
	materials.at(1)->color = { 0,1,0,0};
	materials.at(2)->color = { 0,0,1,0};
	materials.at(3)->color = { 1,1,0,0};
	materials.at(4)->color = { 0,1,1,0};
	materials.at(5)->color = { 1,0,1,0};
	materials.at(6)->color = { 1,1,1,0};
	materials.at(7)->color = { 0,0,0,0};

	std::vector<std::vector<Instance>> cubeInstances(8);

	for (int i = 0; i < cubeInstances.size(); ++i)
	{
		cubeInstances.at(i) = generateRandomInstances(1000);
	}



	std::vector<Instance> samuraiInstances
	{
		Instance
		{
			{{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}
		},

		Instance
		{
			{{1,0,0,0},
			{0,0,-1,0},
			{0,1,0,5},
			{0,0,0,1},}
		},
	};

	std::vector<Instance> eastTowerInstances
	{
		
		Instance
		{
			{{1,0,0,-4},
			{0,0,-1,0},
			{0,1,0,5},
			{0,0,0,1},}
		},
	};

	std::vector<Instance> knightInstances
	{

		Instance
		{
			{{1,0,0,4},
			{0,0,-1,0},
			{0,1,0,5},
			{0,0,0,1},}
		},
	};

	std::vector<Instance> knightHorseInstances
	{

		Instance
		{
			{{1,0,0,6},
			{0,0,-1,0},
			{0,1,0,5},
			{0,0,0,1},}
		},
	};
	for (size_t i = 0; i < samurai.get()->getMeshesCount(); i++)
	{
		engine.addInstancedModel(normalOpaqueInstance, samurai, i, materials.at(4), samuraiInstances);
	}
	for (size_t i = 0; i < eastTower.get()->getMeshesCount(); i++)
	{
		engine.addInstancedModel(normalOpaqueInstance, eastTower, i, materials.at(4), eastTowerInstances);
	}
	for (size_t i = 0; i < knight.get()->getMeshesCount(); i++)
	{
		engine.addInstancedModel(normalOpaqueInstance, knight, i, materials.at(4), knightInstances);
	}
	for (size_t i = 0; i < knightHorse.get()->getMeshesCount(); i++)
	{
		engine.addInstancedModel(normalOpaqueInstance, knightHorse, i, materials.at(4), knightHorseInstances);
	}
	for (size_t i = 0; i < cubeInstances.size(); ++i)
	{
		//engine.addInstancedModel(normalOpaqueInstance, cube, 0, materials.at(i), cubeInstances.at(i));
	}

	engine.addInstancedModel(hologramOpaqueInstance, cube, 0, materials.at(6), cubeInstances.at(0));


	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const engine::DX::float3& position{ 0,0,0 };
	const engine::DX::float3& direction{ 0,0,1 };
	const engine::DX::float3& cameraUp{ 0,1,0 };

	float fov{ 45 };
	float aspect{ (float)windowWidth / windowHeight };
	float zNear{ 0.01 };
	float zFar{ 100 };

	engine.initCamera(position, direction, cameraUp, fov, aspect, zNear, zFar);
	//-----------------------------------------------------------------------------------------------------------------



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
		//scene.render(window);
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
			if (pitchYawRotation)
			{
				xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
				yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			}
			break;
		case WM_LBUTTONDOWN:
			pitchYawRotation = true;
			lastMousePos.x = xPos = static_cast<float>(GET_X_LPARAM(msg.lParam));
			lastMousePos.y = yPos = static_cast<float>(GET_Y_LPARAM(msg.lParam));
			break;
		case WM_LBUTTONUP:
			pitchYawRotation = false;
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

	if (pitchYawRotation)
	{
		RotateCamera(xPos, yPos);
	}
	MoveCamera();

	return true;
}

void ApplicationDX::AddCameraDirection()
{
	//if (cameraMovingDirections[MoveDirection::FORWARD])
	//	cameraDirection += engine.getCameraForward();
	//if (cameraMovingDirections[MoveDirection::BACK])
	//	cameraDirection -= engine.getCameraForward();
	//if (cameraMovingDirections[MoveDirection::LEFT])
	//	cameraDirection -= engine.getCameraRight();
	//if (cameraMovingDirections[MoveDirection::RIGHT])
	//	cameraDirection += engine.getCameraRight();
	//if (cameraMovingDirections[MoveDirection::UP])
	//	cameraDirection += engine.getCameraUp();
	//if (cameraMovingDirections[MoveDirection::DOWN])
	//	cameraDirection -= engine.getCameraUp();


	if (cameraMovingDirections[MoveDirection::FORWARD])
		cameraDirection += {0, 0, 1};
	if (cameraMovingDirections[MoveDirection::BACK])
		cameraDirection -= {0, 0, 1};
	if (cameraMovingDirections[MoveDirection::LEFT])
		cameraDirection -= {1, 0, 0};
	if (cameraMovingDirections[MoveDirection::RIGHT])
		cameraDirection += {1, 0, 0};
	if (cameraMovingDirections[MoveDirection::UP])
		cameraDirection += {0, 1, 0};
	if (cameraMovingDirections[MoveDirection::DOWN])
		cameraDirection -= {0, 1, 0};

}



void ApplicationDX::MoveCamera()
{

	AddCameraDirection();

	//std::cout << cameraDirection.x << '\t' << cameraDirection.y << '\t' << cameraDirection.z << std::endl;
	//std::cout << engine.getCameraPosition().x << '\t' << engine.getCameraPosition().y << '\t' << engine.getCameraPosition().z << std::endl;
	//std::cout << cameraSpeed << std::endl;
	//engine::DX::float4 isCameraMoveVector{ DirectX::XMVectorEqual(cameraDirection, engine::DX::float4{0}) };

	bool isCameraMove{ cameraDirection != engine::DX::float3() };



	if (isCameraMove)
	{


		//engine::DX::float3 moveOffset{ cameraDirection * cameraSpeed * fpsTimer.frameTimeElapsed() * 1000};
		engine::DX::float3 moveOffset{ cameraDirection * cameraSpeed * deltaTime };

		//std::cout << fpsTimer.frameTimeElapsed() << std::endl;
		//std::cout << deltaTime << std::endl;
		engine.moveCamera(moveOffset);
	}

	cameraDirection = engine::DX::float3();

}

void ApplicationDX::RotateCamera(float xPos, float yPos)
{
	/*if (lastMousePos != engine::DX::float2{ xPos, yPos })
	{
		std::cout << xPos << '\t' << yPos << std::endl;
		std::cout << lastMousePos.x << '\t' << lastMousePos.y << std::endl << std::endl;
	}*/

	engine::DX::float2 offset{ lastMousePos.x - xPos, lastMousePos.y - yPos };
	if (std::abs(offset.x) > FLT_EPSILON || std::abs(offset.y) > FLT_EPSILON)
	{
		//std::cout << offset.x << '\t' << offset.y << std::endl;
		engine.rotateCamera({ offset.y * 0.005f, offset.x * 0.005f, 0 });
		//lastMousePos = { xPos, 0 };
	}

	lastMousePos = { xPos, yPos };
}

std::vector<Instance> ApplicationDX::generateRandomInstances(size_t numInstances)
{

	std::vector<Instance> instances(numInstances);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distNeg(-50.f, -1.f);
	std::uniform_real_distribution<float> distPos(1.f, 50.f);



	for (size_t j = 0; j < numInstances; j++)
	{
		float x = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);
		float y = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);
		float z = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);

		instances.at(j) = Instance
		{
			{{1,0,0,x},
			{0,1,0,y},
			{0,0,1,z},
			{0,0,0,1},}
		};

	}

	return instances;
}



