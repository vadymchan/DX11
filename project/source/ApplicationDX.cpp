#include "ApplicationDX.h"
#include <glm/trigonometric.hpp>
#include <random>



void ApplicationDX::Init(const HINSTANCE& appHandle, int windowShowParams)
{
	const int windowWidth{ 1000 };
	const int windowHeight{ 800 };
	const int windowStartX{ 0 };
	const int windowStartY{ 0 };
	const std::wstring normalVertexShaderFileName{ L"normalVertexShader.hlsl" };
	const std::wstring normalGeometryShaderFileName{ L"normalGeometryShader.hlsl" };
	const std::wstring normalPixelShaderFileName{ L"normalPixelShader.hlsl" };
	const std::wstring hologramVertexShaderFileName{ L"hologramVertexShader.hlsl" };
	const std::wstring hologramPixelShaderFileName{ L"hologramPixelShader.hlsl" };
	cameraSpeed = 2.f;
	cameraRotationSpeed = 0.005f;
	cameraMaxPitch = DirectX::XMConvertToRadians(89.0f);
	cameraMinPitch = DirectX::XMConvertToRadians(-89.0f);
#ifdef _DEBUG
	engine::general::initConsole();
#endif

	engine::DX::D3D::Init();


	engine.initWindow(L"DirectX 11 Model", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);
	engine.initRenderer();
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
	ShaderManager::getInstance().addGeometryShader(normalGeometryShaderFileName);
	ShaderManager::getInstance().addPixelShader(hologramPixelShaderFileName);


	// opaque instance
	//---------------------------------------------------------------------------------------------------

	uint32_t normalOpaqueInstance = engine.createOpaqueInstance(normalVertexShaderFileName, normalPixelShaderFileName, normalGeometryShaderFileName);
	uint32_t hologramOpaqueInstance = engine.createOpaqueInstance(hologramVertexShaderFileName, hologramPixelShaderFileName);

	// models
	//---------------------------------------------------------------------------------------------------
	std::shared_ptr<Model> cube = ModelManager::getInstance().getModel(ModelManager::cubeTag);
	std::shared_ptr<Model> samurai = ModelManager::getInstance().getModel("Samurai/Samurai.fbx");
	std::shared_ptr<Model> eastTower = ModelManager::getInstance().getModel("EastTower/EastTower.fbx");
	std::shared_ptr<Model> knight = ModelManager::getInstance().getModel("Knight/Knight.fbx");
	std::shared_ptr<Model> knightHorse = ModelManager::getInstance().getModel("KnightHorse/KnightHorse.fbx");


	std::vector<std::shared_ptr<Material>> materials(8);
	for (auto& material : materials)
	{
		material = std::make_shared<Material>();
	}
	materials.at(0)->color = { 1,0,0,0 };
	materials.at(1)->color = { 0,1,0,0 };
	materials.at(2)->color = { 0,0,1,0 };
	materials.at(3)->color = { 1,1,0,0 };
	materials.at(4)->color = { 0,1,1,0 };
	materials.at(5)->color = { 1,0,1,0 };
	materials.at(6)->color = { 1,1,1,0 };
	materials.at(7)->color = { 0,0,0,0 };

	std::vector<std::vector<std::shared_ptr<Instance>>> cubeInstances(8);

	for (int i = 0; i < cubeInstances.size(); ++i)
	{
		cubeInstances.at(i) = generateRandomInstances(1000);
	}


	std::vector<std::shared_ptr<Instance>> samuraiInstances 
	{
		std::make_shared<Instance>(Instance{
			engine::DX::float4x4
			{{{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}}
		}),

	};

	std::vector<std::shared_ptr<Instance>> eastTowerInstances
	{

		std::make_shared<Instance>(Instance{
			engine::DX::float4x4
			{{{1,0,0,-10},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}}
		}),
	};

	std::vector<std::shared_ptr<Instance>> knightInstances
	{

		std::make_shared<Instance>(Instance{
			engine::DX::float4x4
			{{{1,0,0,10},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}}
		}),
	};

	std::vector<std::shared_ptr<Instance>> knightHorseInstances
	{
		std::make_shared<Instance>(Instance{
			engine::DX::float4x4
			{{{1,0,0,5},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}}
		}),
	};



	std::vector<size_t> samuraiMeshIndices;
	for (size_t i = 0; i < samurai.get()->getMeshesCount(); ++i)
	{
		samuraiMeshIndices.emplace_back(i);
	}
	engine.addInstancedModel(hologramOpaqueInstance, samurai, samuraiMeshIndices, materials.at(1), samuraiInstances);


	std::vector<size_t> eastTowerMeshIndices;
	for (size_t i = 0; i < eastTower.get()->getMeshesCount(); ++i)
	{
		eastTowerMeshIndices.emplace_back(i);
	}
	//engine.addInstancedModel(hologramOpaqueInstance, eastTower, eastTowerMeshIndices, materials.at(1), eastTowerInstances);


	std::vector<size_t> knightMeshIndices;
	for (size_t i = 0; i < knight.get()->getMeshesCount(); ++i)
	{
		knightMeshIndices.emplace_back(i);
	}
	//engine.addInstancedModel(hologramOpaqueInstance, knight, knightMeshIndices, materials.at(1), knightInstances);

	std::vector<size_t> knightHorseMeshIndices;
	for (size_t i = 0; i < knightHorse.get()->getMeshesCount(); ++i)
	{
		knightHorseMeshIndices.emplace_back(i);
	}
	//engine.addInstancedModel(hologramOpaqueInstance, knightHorse, knightHorseMeshIndices, materials.at(1), knightHorseInstances);

	std::vector<size_t> cubeMeshIndices;
	for (size_t i = 0; i < cube.get()->getMeshesCount(); ++i)
	{
		cubeMeshIndices.emplace_back(i);
	}
	for (size_t i = 0; i < cubeInstances.size(); ++i)
	{
		engine.addInstancedModel(normalOpaqueInstance, cube, cubeMeshIndices, materials.at(i), cubeInstances.at(i));
	}

	std::vector<std::shared_ptr<Instance>> cubeInstance
	{
		std::make_shared<Instance>(
			Instance{ engine::DX::float4x4 {
				{1,0,0,0},
				{0,1,0,0},
				{0,0,1,0},
				{0,0,0,1},
			} }),
	};
	engine.addInstancedModel(normalOpaqueInstance, cube, cubeMeshIndices, materials.at(6), cubeInstance);

	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const engine::DX::float3& position{ 0,0,-2 };
	const engine::DX::float3& direction{ 0,0, 1 };
	const engine::DX::float3& cameraUp{ 0,1,0 };

	float fov{ 90 };
	float aspect{ (float)windowWidth / windowHeight };
	float zNear{ 0.1 };
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
			if (pitchYawRotation || rayCasted)
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
	lastMousePos = { xPos, yPos };

	return true;
}

void ApplicationDX::AddCameraDirection()
{
	/*if (cameraMovingDirections[MoveDirection::FORWARD])
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
		cameraDirection -= {0, 1, 0};*/

	if (cameraMovingDirections[MoveDirection::FORWARD])
		cameraDirection += engine.getCameraForward();
	if (cameraMovingDirections[MoveDirection::BACK])
		cameraDirection -= engine.getCameraForward();
	if (cameraMovingDirections[MoveDirection::LEFT])
		cameraDirection -= engine.getCameraRight();
	if (cameraMovingDirections[MoveDirection::RIGHT])
		cameraDirection += engine.getCameraRight();
	if (cameraMovingDirections[MoveDirection::UP])
		cameraDirection += engine.getCameraUp();
	if (cameraMovingDirections[MoveDirection::DOWN])
		cameraDirection -= engine.getCameraUp();

}



bool ApplicationDX::MoveCamera()
{
	AddCameraDirection();
	bool isCameraMove{ cameraDirection != engine::DX::float3() };
	if (isCameraMove)
	{
		engine::DX::float3 moveOffset{ cameraDirection * cameraSpeed * deltaTime };
		engine.moveCamera(moveOffset);
	}
	cameraDirection = engine::DX::float3();
	return isCameraMove;
}

void ApplicationDX::MoveObject(float xPos, float yPos)
{

	if (cameraMovingDirections[MoveDirection::FORWARD])
		objMoveDirection += engine.getCameraForward();
	if (cameraMovingDirections[MoveDirection::BACK])
		objMoveDirection -= engine.getCameraForward();
	if (cameraMovingDirections[MoveDirection::LEFT])
		objMoveDirection -= engine.getCameraRight();
	if (cameraMovingDirections[MoveDirection::RIGHT])
		objMoveDirection += engine.getCameraRight();
	if (cameraMovingDirections[MoveDirection::UP])
		objMoveDirection += engine.getCameraUp();
	if (cameraMovingDirections[MoveDirection::DOWN])
		objMoveDirection -= engine.getCameraUp();

	bool isObjMove{ objMoveDirection != engine::DX::float3() };
	if (isObjMove)
	{
		engine::DX::float3 moveOffset{ objMoveDirection * cameraSpeed * deltaTime };
		engine.moveCapturedObject(moveOffset);
		objMoveDirection = engine::DX::float3();
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
		engine.rotateCamera({ pitch , offset.x * cameraRotationSpeed, 0 });
	}


}

std::vector<std::shared_ptr<Instance>> ApplicationDX::generateRandomInstances(size_t numInstances)
{

	std::vector<std::shared_ptr<Instance>> instances(numInstances);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distNeg(-50.f, -1.f);
	std::uniform_real_distribution<float> distPos(1.f, 50.f);



	for (size_t j = 0; j < numInstances; j++)
	{
		float x = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);
		float y = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);
		float z = std::generate_canonical<float, 10>(gen) > 0.5 ? distPos(gen) : distNeg(gen);

		instances.at(j) = std::make_shared<Instance>(Instance{
			engine::DX::float4x4
			{{{1,0,0,x},
			{0,1,0,y},
			{0,0,1,z},
			{0,0,0,1},}}
		});

	}

	return instances;
}



