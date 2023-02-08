#include "ApplicationDX.h"
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
	cameraMoveCoef = 0.5f;
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

	std::shared_ptr<Material> mat = std::make_shared<Material>();
	mat->color = { 1.0,0,0,0 };

	std::vector<Instance> instances
	{
		Instance{DirectX::XMMatrixIdentity()}
	};

	engine.addInstancedModel(cube, 0, mat, instances);

	//engine::DX::VertexBuffer opaqueInstanceVertexBuffer;

	std::cout << sizeof(Mesh::Vertex);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc
	{
		//model vertex buffer
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(0), D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, MODEL_DATA_INPUT_SLOT_0, Mesh::Vertex::alignedByteOffsets.at(1), D3D11_INPUT_PER_VERTEX_DATA, 0},

		//instance vertex buffer
		{"INSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, 0,							 D3D11_INPUT_PER_INSTANCE_DATA, 0},
		{"INSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 0},
		{"INSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 0},
		{"INSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, INSTANCE_INPUT_SLOT_1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 0},
	};

	ShaderManager::getInstance().addVertexShader(L"vertexShader.hlsl", inputElementDesc);
	ShaderManager::getInstance().addPixelShader(L"pixelShader.hlsl");

	engine.setVertexShader(L"vertexShader.hlsl");
	engine.setPixelShader(L"pixelShader.hlsl");

	//camera
	//-----------------------------------------------------------------------------------------------------------------
	const DirectX::SimpleMath::Vector3& position{0,0,-20};
	const DirectX::SimpleMath::Vector3& direction{0,0,1};
	const DirectX::SimpleMath::Vector3& cameraUp{0,1,0};

	float fov{45};
	float aspect{(float)windowWidth /windowHeight};
	float zNear{ 0.01 };
	float zFar{ 100 };

	engine.initCamera(position, direction, cameraUp, fov, aspect, zNear, zFar);
	//-----------------------------------------------------------------------------------------------------------------

}

void ApplicationDX::Run()
{
	while (true)
	{
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
	float xPos{};
	float yPos{};

	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		switch (msg.message)
		{
		case WM_MOUSEMOVE:
			if (pitchYawRotation)
			{
				xPos = static_cast<float>(GET_X_LPARAM(msg.wParam));
				yPos = static_cast<float>(GET_Y_LPARAM(msg.wParam));
			}
			break;
		case WM_LBUTTONDOWN:
			pitchYawRotation = true;
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

	
	MoveCamera();

	return true;
}

void ApplicationDX::AddCameraDirection()
{
	if (cameraMovingDirections[MoveDirection::FORWARD])
		cameraDirection = DirectX::XMVectorAdd(cameraDirection, engine.getCameraForward());
	if (cameraMovingDirections[MoveDirection::BACK])
		cameraDirection = DirectX::XMVectorAdd(cameraDirection, DirectX::XMVectorNegate(engine.getCameraForward()));
	if (cameraMovingDirections[MoveDirection::LEFT])
		cameraDirection = DirectX::XMVectorAdd(cameraDirection, DirectX::XMVectorNegate(engine.getCameraRight()));
	if (cameraMovingDirections[MoveDirection::RIGHT])
		cameraDirection = DirectX::XMVectorAdd(cameraDirection, engine.getCameraRight());
	if (cameraMovingDirections[MoveDirection::UP])
		cameraDirection = DirectX::XMVectorAdd(cameraDirection, engine.getCameraUp());
	if (cameraMovingDirections[MoveDirection::DOWN])
		cameraDirection = DirectX::XMVectorAdd(cameraDirection, DirectX::XMVectorNegate(engine.getCameraUp()));

}
	


void ApplicationDX::MoveCamera()
{

	AddCameraDirection();

	//DirectX::XMVECTOR isCameraMoveVector{ DirectX::XMVectorEqual(cameraDirection, DirectX::XMVECTOR{0}) };
	bool isCameraMove{ cameraDirection.m128_f32[0] == 0 && cameraDirection.m128_f32[1] == 0 && cameraDirection.m128_f32[2] == 0 };

	if (isCameraMove)
	{
		DirectX::XMVECTOR moveOffset{ DirectX::XMVectorScale(cameraDirection, cameraMoveCoef * fpsTimer.frameTimeElapsed())};
		engine.moveCamera(std::move(moveOffset));
	}

	cameraDirection = DirectX::XMVECTOR();

}

void ApplicationDX::RotateCamera()
{

	
}



