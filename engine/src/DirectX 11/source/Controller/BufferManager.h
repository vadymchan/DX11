#pragma once
#include <array>
#include "../D3D/D3D.h"

namespace engine::DX
{
	//constant buffer
	constexpr UINT PER_VIEW_SHADER = 0; //in vertex shader
	constexpr UINT INV_PER_VIEW_SHADER = 1; //in vertex shader
	constexpr UINT MESH_TO_MODEL_SHADER = 2; //in vertex shader
	constexpr UINT MATERIAL_NORMAL_SHADER = 0; //in pixel shader
	constexpr UINT PER_VIEW_PIXEL_HOLOGRAM_SHADER = 0; //in pixel shader
	constexpr UINT PER_DRAW_HOLOGRAM_SHADER = 1; //in pixel and vertex shader
	constexpr UINT PER_VIEW_SHADOW_MAP_2D = 0;
	constexpr UINT PER_VIEW_SHADOW_MAP_3D = 0;
	constexpr UINT PER_FRAME_GLOBAL = 4;
	constexpr UINT PER_FRAME_IBL_BIND_SLOT = 5;
	constexpr UINT SELECTED_SAMPLER_STATE_3 = 3;
	constexpr UINT SCREEN_RESOLUTION = 0;

	//vertex buffer
	constexpr UINT MODEL_DATA_INPUT_SLOT_0 = 0;
	constexpr UINT INSTANCE_INPUT_SLOT_1 = 1;
	constexpr UINT PARTICLE_DATA_INPUT_SLOT_2 = 0;
	constexpr UINT SKYBOX_VERTEX_BUFFER_DATA_0 = 0;

	
	void UnbindVertexBuffer(UINT slot);



}

