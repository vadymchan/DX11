#include "BufferManager.h"

namespace engine::DX
{
	void UnbindVertexBuffer(UINT slot)
	{
		ID3D11Buffer* pBufferNull = NULL;
		UINT stride = 0;
		UINT offset = 0;
		g_devcon->IASetVertexBuffers(slot, 1, &pBufferNull, &stride, &offset);
	}
}
