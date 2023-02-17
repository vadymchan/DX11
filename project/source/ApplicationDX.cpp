#include "ApplicationDX.h"
#include <glm/trigonometric.hpp>
using Instance = engine::DX::OpaqueInstances::Instance;
using Material = engine::DX::OpaqueInstances::Material;
using engine::DX::Model;
using engine::DX::Mesh;
using engine::DX::MeshSystem;
using engine::DX::ModelManager;
using engine::DX::ShaderManager;


void ApplicationDX::Init(const HINSTANCE& appHandle, int windowShowParams)
{
	const int windowWidth{ 600 };
	const int windowHeight{ 400 };
	const int windowStartX{ 0 };
	const int windowStartY{ 0 };
	cameraSpeed = 2.f;
#ifdef _DEBUG
	engine::general::initConsole();
#endif
	//engine::DX::VertexType vertices[] =
	//{
	//	{{0.0f, 0.5f, 0.0f,    1.0f},{1.0f, 0.0f, 0.0f, 1.0f}},
	//	{{0.45f, -0.5, 0.0f,   1.0f},{0.0f, 1.0f, 0.0f, 1.0f}},
	//	{{-0.45f, -0.5f, 0.0f, 1.0f},{0.0f, 0.0f, 1.0f, 1.0f}}
	//};

	engine::DX::D3D::Init();

	

//	std::vector<engine::DX::Scene::Position> positions
//	{
//		engine::DX::Scene::Position{DirectX::XMFLOAT4{0.0f, 0.5f, 0.0f,    1.0f}},
//		engine::DX::Scene::Position{DirectX::XMFLOAT4{0.45f, -0.5, 0.0f,   1.0f}},
//		engine::DX::Scene::Position{DirectX::XMFLOAT4{-0.45f, -0.5f, 0.0f, 1.0f}}
//	};
//	std::vector<engine::DX::Scene::Color> colors
//	{
//		engine::DX::Scene::Color{DirectX::XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}},
//		engine::DX::Scene::Color{DirectX::XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}},
//		engine::DX::Scene::Color{DirectX::XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}}
//	};
//
//	scene.positions = positions;
//	scene.colors = colors;
//
//
//
//	
//	window.initWindow(L"DirectX 11 Triangle", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);
	engine.initWindow(L"DirectX 11 Model", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);
//	
//	scene.initPipeline();
//	scene.addVertex(engine::DX::VertexType({ {0.0f, 0.5f, 0.0f,    0.0f},{1.0f, 0.0f, 0.0f, 1.0f} }));
//	scene.addVertex(engine::DX::VertexType({ {0.45f, -0.5, 0.0f,   0.0f},{0.0f, 1.0f, 0.0f, 1.0f} }));
//	scene.addVertex(engine::DX::VertexType({ {-0.45f, -0.5f, 0.0f, 0.0f},{0.0f, 0.0f, 1.0f, 1.0f} }));
//	scene.initVertexBuffer();

	std::shared_ptr<Model> cube = ModelManager::getInstance().loadCube();
	//std::shared_ptr<Model> cube1 = ModelManager::getInstance().loadAssimp("dald");

	std::shared_ptr<Material> mat = std::make_shared<Material>();
	mat->color = { 1.0,0,0,0 };


		


	std::vector<Instance> instances
	{
		
		Instance
		{
			{{1,0,0,0},
			{0,1,0,0},
			{0,0,1,10},
			{0,0,0,1},}
		},
		Instance
		{
			{{0.8,-0.6,0,0.8},
			{0.6,0.8,0,0.4},
			{0,0,1,2},
			{0,0,0,1},}
		},
		Instance
		{
			{{1,0,0,10},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}
		},
		Instance
		{
			{{1,0,0,-10},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1},}
		},
		Instance
		{
			{{1,0,0,0},
			{0,1,0,0},
			{0,0,1,-10},
			{0,0,0,1},}
		},
		
	};

	engine.addInstancedModel(cube, 0, mat, instances);

	//engine::DX::VertexBuffer opaqueInstanceVertexBuffer;


	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},

		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
	};

	

	ShaderManager::getInstance().addVertexShader(L"vertexShader.hlsl", inputElementDesc);
	ShaderManager::getInstance().addPixelShader(L"pixelShader.hlsl");

	engine.setVertexShader(L"vertexShader.hlsl");
	engine.setPixelShader(L"pixelShader.hlsl");

	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const engine::DX::float3& position{0,0,0};
	const engine::DX::float3& direction{1,0,0};
	const engine::DX::float3& cameraUp{0,1,0};

	float fov{45};
	float aspect{(float)windowWidth /windowHeight};
	float zNear{ 0.01 };
	float zFar{ 100 };

	engine.initCamera(position, direction, cameraUp, fov, aspect, zNear, zFar);
	//-----------------------------------------------------------------------------------------------------------------


	DirectX::XMVECTOR pos = { 0, 0, 0 };
	DirectX::XMVECTOR dir = { 1, 0, 0 };
	DirectX::XMVECTOR up = { 0, 1, 0 };
	auto t = DirectX::XMMatrixLookToLH(pos, dir, up);


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
	float xPos{lastMousePos.x};
	float yPos{lastMousePos.y};

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
				cameraSpeed *= 5;
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
				cameraSpeed /= 5;
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


	//if (cameraMovingDirections[MoveDirection::FORWARD])
	//	cameraDirection += {0,0,1};
	//if (cameraMovingDirections[MoveDirection::BACK])
	//	cameraDirection -= {0,0,1};
	//if (cameraMovingDirections[MoveDirection::LEFT])
	//	cameraDirection -= {1,0,0};
	//if (cameraMovingDirections[MoveDirection::RIGHT])
	//	cameraDirection += {1,0,0};
	//if (cameraMovingDirections[MoveDirection::UP])
	//	cameraDirection += {0,1,0};
	//if (cameraMovingDirections[MoveDirection::DOWN])
	//	cameraDirection -= {0,1,0};

}
	


void ApplicationDX::MoveCamera()
{

	AddCameraDirection();

	//std::cout << cameraDirection.x << '\t' << cameraDirection.y << '\t' << cameraDirection.z << std::endl;
	//std::cout << engine.getCameraPosition().x << '\t' << engine.getCameraPosition().y << '\t' << engine.getCameraPosition().z << std::endl;

	//engine::DX::float4 isCameraMoveVector{ DirectX::XMVectorEqual(cameraDirection, engine::DX::float4{0}) };

	bool isCameraMove{ cameraDirection != engine::DX::float3()};

	

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
		engine.rotateCamera({offset.y  * 0.005f, offset.x * 0.005f, 0});
		//lastMousePos = { xPos, 0 };
	}
	
		lastMousePos = { xPos, yPos };
}



