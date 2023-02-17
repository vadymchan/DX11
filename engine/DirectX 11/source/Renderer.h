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
			window.setViews();
			camera.setCameraBuffer();
			MeshSystem::getInstance().render();
			window.flush();
			window.clearDepthStencil();
		}
	private:

	};

}