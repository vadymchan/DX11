#pragma once


#include <string>
#include <unordered_map>
#include <array>
#include "../Buffer/IndexBuffer.h"
#include "../Buffer/VertexBuffer.h"
#include "../Buffer/ConstantBuffer.h"

namespace engine::DX
{
#define PER_VIEW_SHADER 0 //in vertex shader
#define INV_PER_VIEW_SHADER 1 //in vertex shader
#define MESH_TO_MODEL_SHADER 2 //in vertex shader
#define MATERIAL_NORMAL_SHADER 0 //in pixel shader
#define PER_VIEW_PIXEL_HOLOGRAM_SHADER 0 //in pixel shader
#define PER_DRAW_HOLOGRAM_SHADER 1 //in pixel and vertex shader
#define MODEL_DATA_INPUT_SLOT_0 0
#define INSTANCE_INPUT_SLOT_1 1
#define SKYBOX_VERTEX_BUFFER_DATA_0 0
#define SELECTED_SAMPLER_STATE_3 3

	class BufferManager
	{
	public:
		static BufferManager& getInstance()
		{
			static BufferManager instance;
			return instance;
		}

		bool isInputSlotTaken(size_t index) const
		{
			return vertexInputSlots.at(index);
		}

	private:
		std::array<bool, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> vertexInputSlots;
	};

}

