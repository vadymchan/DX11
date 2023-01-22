#include "Ray Casting/windows/WindowRC.h"
#include "Application.h"

#include <d3d11.h>
#include <d3dx11.h>
//#include <d3d10.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
//#pragma comment (lib, "d3d10.lib")

struct Vertex
{
	FLOAT X, Y, Z;
	float Color[4];
};

IDXGISwapChain* swapchain;
ID3D11Device* dev;
ID3D11DeviceContext* devcon;
ID3D11RenderTargetView* backBuffer;

ID3D11VertexShader* pVS;
ID3D11PixelShader* pPS;

ID3D11Buffer* pVBuffer;

ID3D11InputLayout* pLayout;

void InitD3D(const HWND& hWnd);
void CleanD3D();


Vertex vertices[] =
{
	{0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
	{0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
	{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
};

void renderFrame()
{
	float color[4] = { 0.0f, 0.6f, 0.4f, 1.f };
	devcon->ClearRenderTargetView(backBuffer, color);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->Draw(3, 0);

	swapchain->Present(0,0);
}

int WINAPI WinMain(_In_ HINSTANCE appHandle, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR cmdLine, _In_ int windowShowParams)
{
	/*RC::engine::Window window;
	window.initWindow(L"DX Triangle", 200, 200, SCREEN_WIDTH, SCREEN_HEIGHT, appHandle, windowShowParams);

	InitD3D(window.GetHWND());

	MSG msg;

	while (true)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				break;
			}

		}
		renderFrame();

	}

	CleanD3D();*/

	Application application;
	application.Init(appHandle, windowShowParams);
	application.Run();

	return EXIT_SUCCESS;
}


void InitD3D(const HWND& hWnd)
{

	DXGI_SWAP_CHAIN_DESC sdesc;

	ZeroMemory(&sdesc, sizeof(sdesc));

	sdesc.BufferCount = 2;
	sdesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sdesc.OutputWindow = hWnd;
	sdesc.SampleDesc.Count = 1;
	sdesc.Windowed = true;
	sdesc.BufferDesc.Width = SCREEN_WIDTH;
	sdesc.BufferDesc.Height = SCREEN_HEIGHT;
	sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&sdesc,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	//render target

	ID3D11Texture2D* pBackBuffer;
	swapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer); // get address of back buffer

	dev->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
	pBackBuffer->Release();

	devcon->OMSetRenderTargets(1, &backBuffer, NULL);


	//viewport

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);


	//shaders
	
//
//	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//	flags |= D3DCOMPILE_DEBUG; // add more debug output
//#endif
//	ID3DBlob* vs_blob_ptr = NULL, * ps_blob_ptr = NULL, * error_blob = NULL;
//
//	// COMPILE VERTEX SHADER
//	HRESULT hr = D3DCompileFromFile(
//		L"shaders.hlsl",
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"vs_main",
//		"vs_5_0",
//		flags,
//		0,
//		&vs_blob_ptr,
//		&error_blob);
//	if (FAILED(hr)) {
//		if (error_blob) {
//			OutputDebugStringA((char*)error_blob->GetBufferPointer());
//			error_blob->Release();
//		}
//		if (vs_blob_ptr) { vs_blob_ptr->Release(); }
//		assert(false);
//	}
//
//	// COMPILE PIXEL SHADER
//	hr = D3DCompileFromFile(
//		L"shaders.hlsl",
//		nullptr,
//		D3D_COMPILE_STANDARD_FILE_INCLUDE,
//		"ps_main",
//		"ps_5_0",
//		flags,
//		0,
//		&ps_blob_ptr,
//		&error_blob);
//	if (FAILED(hr)) {
//		if (error_blob) {
//			OutputDebugStringA((char*)error_blob->GetBufferPointer());
//			error_blob->Release();
//		}
//		if (ps_blob_ptr) { ps_blob_ptr->Release(); }
//		assert(false);
//	}



	ID3D10Blob* VS, * PS;


	D3DX11CompileFromFile(L"C:\\Users\\cocon\\source\\repos\\vchan hw 3\\x64\\Debug\\shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"C:\\Users\\cocon\\source\\repos\\vchan hw 3\\x64\\Debug\\shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);
	
	//buffer

	D3D11_BUFFER_DESC vdesc;
	ZeroMemory(&vdesc, sizeof(vdesc));

	vdesc.Usage = D3D11_USAGE_DYNAMIC;
	vdesc.ByteWidth = sizeof(Vertex) * 3;
	vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	dev->CreateBuffer(&vdesc, NULL, &pVBuffer);

	//map (copy) resources
	D3D11_MAPPED_SUBRESOURCE maps;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &maps);
	memcpy(maps.pData, vertices, sizeof(vertices));
	devcon->Unmap(pVBuffer, NULL);

	//input layout

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);


	

}

void CleanD3D()
{

	pVS->Release();
	pPS->Release();
	swapchain->SetFullscreenState(FALSE, NULL);
	swapchain->Release();
	backBuffer->Release();
	dev->Release();
	devcon->Release();
}