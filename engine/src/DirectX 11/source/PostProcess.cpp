#include "PostProcess.h"


namespace engine::DX
{
	const UINT PostProcess::POST_PROCESS_SAMPLE_STATE_SLOT = 0;
	const UINT PostProcess::POST_PROCESS_CONST_BUFFER_SLOT = 0;
	const std::wstring PostProcess::POST_PROCESS_VERTEX_SHADER_PATH = L"hdr/hdrVertexShader.hlsl";
	const std::wstring PostProcess::POST_PROCESS_PIXEL_SHADER_PATH = L"hdr/hdrPixelShader.hlsl";
}
