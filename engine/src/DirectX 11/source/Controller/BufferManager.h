#pragma once
#include <array>
#include "../D3D/D3D.h"

namespace engine::DX
{

	const UINT PER_VIEW_SHADER = 0; //in vertex shader
	const UINT INV_PER_VIEW_SHADER = 1; //in vertex shader
	const UINT MESH_TO_MODEL_SHADER = 2; //in vertex shader
	const UINT MATERIAL_NORMAL_SHADER = 0; //in pixel shader
	const UINT PER_VIEW_PIXEL_HOLOGRAM_SHADER = 0; //in pixel shader
	const UINT PER_DRAW_HOLOGRAM_SHADER = 1; //in pixel and vertex shader
	const UINT MODEL_DATA_INPUT_SLOT_0 = 0;
	const UINT INSTANCE_INPUT_SLOT_1 = 1;
	const UINT SKYBOX_VERTEX_BUFFER_DATA_0 = 0;
	const UINT SELECTED_SAMPLER_STATE_3 = 3;
	const UINT PER_FRAME_GLOBAL = 4;
	const UINT PER_FRAME_IBL_BIND_SLOT = 5;

	const UINT PER_VIEW_SHADOW_MAP_2D = 0;
	const UINT PER_VIEW_SHADOW_MAP_3D = 0;
	
	const UINT POINT_LIGHT_GLOBAL = 0;
	



}

