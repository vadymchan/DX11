#pragma once
#include "D3D/D3D.h"
#include "Buffer/ConstantBuffer.h"
#include "Texture/Texture2D.h"
#include "Controller/ShaderManager.h"

namespace engine::DX
{

	
	class PostProcess
	{
	public:

		void init(D3D11_TEXTURE2D_DESC backBufferTex, float EV100 = 12.0f, float Gamma = 2.2f)
		{
			backBufferTex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			backBufferTex.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = backBufferTex.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			m_HDRtexture.createTextureFromMemory(POST_PROCESS_SAMPLE_STATE_SLOT, backBufferTex, nullptr, srvDesc);

			m_constBuffer.initBuffer(POST_PROCESS_CONST_BUFFER_SLOT, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);

			ShaderManager::getInstance().addVertexShader(POST_PROCESS_VERTEX_SHADER_PATH, {});
			ShaderManager::getInstance().addPixelShader(POST_PROCESS_PIXEL_SHADER_PATH);

			m_Gamma = Gamma;
			m_EV100 = EV100;

		}

		void resolve(ID3D11RenderTargetView* dst)
		{
			//g_devcon->PSSetShaderResources(0, 1, &src);
			m_HDRtexture.bind();
			g_devcon->OMSetRenderTargets(1, &dst, nullptr);

			ShaderManager::getInstance().getVertexShader(POST_PROCESS_VERTEX_SHADER_PATH)->bind();
			ShaderManager::getInstance().getPixelShader(POST_PROCESS_PIXEL_SHADER_PATH)->bind();
			m_constBuffer.setBufferData({ {m_Gamma, m_EV100, 0,0 } });
			m_constBuffer.setPixelShaderBuffer();

			g_devcon->Draw(4, 0);

			g_devcon->PSSetShaderResources(0, 1, nullptr);

		}

	private:

		float m_EV100;
		float m_Gamma;

		const static UINT POST_PROCESS_SAMPLE_STATE_SLOT;
		const static UINT POST_PROCESS_CONST_BUFFER_SLOT;
		const static std::wstring POST_PROCESS_VERTEX_SHADER_PATH;
		const static std::wstring POST_PROCESS_PIXEL_SHADER_PATH;

		struct ConstBuffer
		{
			float EV100;
			float Gamma;
			float padding[2];
		};

		ConstantBuffer<ConstBuffer> m_constBuffer;
		Texture2D m_HDRtexture;

	};

	const UINT PostProcess::POST_PROCESS_SAMPLE_STATE_SLOT = 0;
	const UINT PostProcess::POST_PROCESS_CONST_BUFFER_SLOT = 0;
	const std::wstring PostProcess::POST_PROCESS_VERTEX_SHADER_PATH = L"hdr/hdrVertexShader.hlsl";
	const std::wstring PostProcess::POST_PROCESS_PIXEL_SHADER_PATH = L"hdr/hdrPixelShader.hlsl";
}

