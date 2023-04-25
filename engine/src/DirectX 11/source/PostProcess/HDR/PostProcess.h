#pragma once
#include "../../D3D/D3D.h"
#include "../../Buffer/ConstantBuffer.h"
#include "../../Texture/Texture2D.h"
#include "../../Controller/ShaderManager.h"
#include "../../Controller/ImGuiManager.h"

namespace engine::DX
{
	const std::string ImGuiEv100 = "EV100";
	
	class PostProcess
	{
	public:

		void init(float EV100 = 2.f, float Gamma = 2.2f);

		float getEV100() const;

		void setEV100(float EV100);

		void updateConstantData();

		void resolve(ID3D11ShaderResourceView* src, ID3D11RenderTargetView* dst);

		const static UINT POST_PROCESS_SAMPLE_STATE_SLOT;

	private:

		float m_EV100;
		float m_Gamma;
		bool m_updateConstantBuffer;

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

