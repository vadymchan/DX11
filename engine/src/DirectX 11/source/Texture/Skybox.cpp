#include "Skybox.h"

namespace engine::DX
{
	const std::wstring Skybox::VERTEX_SHADER_PATH = L"skybox/skyboxVertexShader.hlsl";
	const std::wstring Skybox::PIXEL_SHADER_PATH = L"skybox/skyboxPixelShader.hlsl";

	void Skybox::initSkybox(const std::wstring& textureFileName)
	{
		m_textureFileName = textureFileName;

		//initing skybox shaders
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout{};
		ShaderManager::getInstance().addVertexShader(VERTEX_SHADER_PATH, inputLayout);
		ShaderManager::getInstance().addPixelShader(PIXEL_SHADER_PATH);

		//initing skybox texture
		if (!m_textureFileName.empty())
		{
			D3D11_TEXTURE2D_DESC textureDesc;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
			TextureManager::getInstance().addTexture2D(m_textureFileName, 0, textureDesc);
		}

	}

	void Skybox::render(Camera& camera)
	{

		//turn off depth test
		ComPtr<ID3D11DepthStencilState> originalDepthStencilState;
		UINT stencilRef;
		g_devcon->OMGetDepthStencilState(originalDepthStencilState.GetAddressOf(), &stencilRef);
		D3D11_DEPTH_STENCIL_DESC skyboxDepthStencilDesc;
		originalDepthStencilState->GetDesc(&skyboxDepthStencilDesc);
		skyboxDepthStencilDesc.DepthEnable = true;
		skyboxDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		skyboxDepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
		ComPtr<ID3D11DepthStencilState> skyboxDepthStencilState;
		if (skyboxDepthStencilState.Get() == nullptr)
		{
			HRESULT result = g_device->CreateDepthStencilState(&skyboxDepthStencilDesc, skyboxDepthStencilState.GetAddressOf());
			if (FAILED(result))
			{
				PrintError(result, L"Depth disabled in skybox rendering");
			}
		}
		//TODO::add check for D3D11_DEPTH_STENCIL_DESC is the same in both  skyboxDepthStencilState and original one

		g_devcon->OMSetDepthStencilState(skyboxDepthStencilState.Get(), stencilRef);

		camera.setInvCameraBufferVertexShader();
		ShaderManager::getInstance().getVertexShader(VERTEX_SHADER_PATH)->bind();
		ShaderManager::getInstance().getPixelShader(PIXEL_SHADER_PATH)->bind();
		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		g_devcon->HSSetShader(nullptr, nullptr, 0);
		g_devcon->DSSetShader(nullptr, nullptr, 0);
		g_devcon->GSSetShader(nullptr, nullptr, 0);

		TextureManager::getInstance().getTexture2D(m_textureFileName).bind();
		g_devcon->Draw(3, 0);

		g_devcon->OMSetDepthStencilState(originalDepthStencilState.Get(), stencilRef);


	}

}