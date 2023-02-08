#pragma once


#include <string>
#include <unordered_map>
#include <array>
#include "../Buffer/IndexBuffer.h"
#include "../Buffer/VertexBuffer.h"
#include "../Buffer/ConstantBuffer.h"

namespace engine::DX
{
#define CONSTANT_REGISTER_0 0
#define MODEL_DATA_INPUT_SLOT_0 0
#define INSTANCE_INPUT_SLOT_1 1


	class BufferManager
	{
	public:
		static BufferManager& getInstance()
		{
			static BufferManager instance;
			return instance;
		}
		/*template <typename T>
		void addVertexBuffer(const std::string& tag, const VertexBuffer<T>& vertexBuffer)
		{
			auto buffer = vertexBuffers.find(tag);
			if (buffer != vertexBuffers.end())
			{
				std::cerr << "Vertex Buffer with tag: " << tag << " was already created. The data for this vertex buffer was updated.\n";
			}
			vertexBuffers[tag] = std::make_shared<VertexBuffer>(vertexBuffer);

		}

		bool getVertexBuffer(const std::string& tag, std::shared_ptr<VertexBuffer>& vertexBuffer)
		{
			auto buffer = vertexBuffers.find(tag);
			if (buffer == vertexBuffers.end())
			{
				std::cerr << "Vertex Buffer by given tag \"" << tag << "\" was not found (it was not initialized)\n";
				return false;
			}
			vertexBuffer = buffer->second;
			return true;
		}*/

		bool isInputSlotTaken(size_t index) const
		{
			return vertexInputSlots.at(index);
		}

	private:
		std::array<bool, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT> vertexInputSlots;
		/*std::unordered_map<std::string, std::shared_ptr<VertexBuffer>> vertexBuffers;
		std::unordered_map<std::string, std::shared_ptr<IndexBuffer>> indexBuffers;
		std::unordered_map<std::string, std::shared_ptr<ConstantBuffer>> constantBuffers;*/
	};

}

