#pragma once
#include "../window/WindowDX.h"
#include "Camera.h"
#include "MeshSystem/MeshSystem.h"

namespace engine::DX
{
	class Renderer
	{
	public:
		
		void render(Window& window, Camera& camera) 
		{
			window.clearWindow();
			g_devcon->OMSetRenderTargets(1, window.GetppRenderTargetView(), nullptr);
			camera.setCameraBuffer();
			MeshSystem::getInstance().render();
			window.flush();
		}
	private:

	};

}