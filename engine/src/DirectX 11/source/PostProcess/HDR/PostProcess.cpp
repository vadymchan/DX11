#include "PostProcess.h"


namespace engine::DX
{
	const UINT PostProcess::POST_PROCESS_SAMPLE_STATE_SLOT = 0;
	const UINT PostProcess::POST_PROCESS_CONST_BUFFER_SLOT = 0;
	const std::wstring PostProcess::POST_PROCESS_VERTEX_SHADER_PATH = L"hdr/hdrVertexShader.hlsl";
	const std::wstring PostProcess::POST_PROCESS_PIXEL_SHADER_PATH = L"hdr/hdrPixelShader.hlsl";

	void PostProcess::init(float EV100, float Gamma)
	{
		m_constBuffer.initBuffer(POST_PROCESS_CONST_BUFFER_SLOT, D3D11_USAGE_DYNAMIC, D3D10_CPU_ACCESS_WRITE);

		ShaderManager::getInstance().addVertexShader(POST_PROCESS_VERTEX_SHADER_PATH, {});
		ShaderManager::getInstance().addPixelShader(POST_PROCESS_PIXEL_SHADER_PATH);

		m_constBuffer.setBufferData({ { Gamma, EV100, 0,0 } });
		m_constBuffer.Buffer::setBuffer();
		m_Gamma = Gamma;
		m_EV100 = EV100;

		ImGuiManager::getInstance().AddValueChangedCallback([this](const std::string& identifier, float value) {
			if (identifier == ImGuiEv100) {
				setEV100(value);
			}
			});

		m_updateConstantBuffer = false;
	}

	float PostProcess::getEV100() const
	{
		return m_EV100;
	}

	void PostProcess::setEV100(float EV100)
	{
		m_EV100 = EV100;
		m_updateConstantBuffer = true;
	}

	void PostProcess::updateConstantData()
	{
		if (m_updateConstantBuffer)
		{
			m_constBuffer.updateData({ { m_EV100 ,m_Gamma, 0,0 } });
			m_updateConstantBuffer = false;
		}
	}

	void PostProcess::resolve(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst)
	{
		updateConstantData();

		g_devcon->PSSetShaderResources(0, 1, &src);

		g_devcon->OMSetRenderTargets(1, &dst, nullptr);

		ShaderManager::getInstance().getVertexShader(POST_PROCESS_VERTEX_SHADER_PATH)->bind();
		ShaderManager::getInstance().getPixelShader(POST_PROCESS_PIXEL_SHADER_PATH)->bind();

		m_constBuffer.setPixelShaderBuffer();

		g_devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		g_devcon->Draw(4, 0);


		ID3D11ShaderResourceView* nullSRV = nullptr;
		g_devcon->PSSetShaderResources(0, 1, &nullSRV);

		ImGuiManager::getInstance().RenderSlider(ImGuiEv100, m_EV100, 0, 5);
	}
}
