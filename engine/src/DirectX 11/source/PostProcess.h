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

		void init(float EV100 = 14.f, float Gamma = 2.2f)
		{
			

			m_constBuffer.initBuffer(POST_PROCESS_CONST_BUFFER_SLOT, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);

			ShaderManager::getInstance().addVertexShader(POST_PROCESS_VERTEX_SHADER_PATH, {});
			ShaderManager::getInstance().addPixelShader(POST_PROCESS_PIXEL_SHADER_PATH);

			m_Gamma = Gamma;
			m_EV100 = EV100;

		}

		void resolve(ID3D11ShaderResourceView* src,ID3D11RenderTargetView* dst)
		{
			g_devcon->PSSetShaderResources(0, 1, &src);
			
			g_devcon->OMSetRenderTargets(1, &dst, nullptr);

			ShaderManager::getInstance().getVertexShader(POST_PROCESS_VERTEX_SHADER_PATH)->bind();
			ShaderManager::getInstance().getPixelShader(POST_PROCESS_PIXEL_SHADER_PATH)->bind();
			m_constBuffer.setBufferData({ {m_Gamma, m_EV100, 0,0 } });
			m_constBuffer.setPixelShaderBuffer();

			g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			g_devcon->Draw(4, 0);
			

			ID3D11ShaderResourceView *nullSRV = nullptr;
			g_devcon->PSSetShaderResources(0, 1, &nullSRV);

		}

		const static UINT POST_PROCESS_SAMPLE_STATE_SLOT;

	private:

		float m_EV100;
		float m_Gamma;

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


	};

}

