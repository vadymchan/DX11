#include "ApplicationDX.h"

void ApplicationDX::Init(const HINSTANCE& appHandle, int windowShowParams)
{
	const int windowWidth{ 600 };
	const int windowHeight{ 400 };
	const int windowStartX{ 0 };
	const int windowStartY{ 0 };

	window.initConsole();

	engine::DX::Vertex vertices[] =
	{
		{0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
		{0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
		{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
	};

	std::cout << sizeof(vertices) << std::endl;

	engine::DX::D3D::Init();

	window.initWindow(L"DirectX 11 Triangle", windowStartX, windowStartY, windowWidth, windowHeight, appHandle, windowShowParams);

	scene.initPipeline();
	scene.addVertex(engine::DX::Vertex(0.0f, 0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f }));
	scene.addVertex(engine::DX::Vertex(0.45f, -0.5, 0.0f, { 0.0f, 1.0f, 0.0f, 1.0f }));
	scene.addVertex(engine::DX::Vertex(-0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f }));
	scene.initVertexBuffer();

}

void ApplicationDX::Run()
{
	while (true)
	{
		if (!ProcessInputs())
		{
			return;
		}
		scene.render(window);

	}
}


bool ApplicationDX::ProcessInputs()
{
	MSG msg;

	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		switch (msg.message)
		{
		case WM_QUIT:
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return true;
}


