#pragma once
#include "source/MeshSystem/MeshSystem.h"
#include "source/Controller/ModelManager.h"
#include "source/Controller/BufferManager.h"
#include "source/Controller/ShaderManager.h"
#include "source/MeshSystem/OpaqueInstances.h"
#include "window/WindowDX.h"
#include "source/Camera.h"
#include "source/Renderer.h"




namespace engine::DX
{

	class Engine
	{
	public:
		

		void initWindow(const LPCWSTR& title, int xStart, int yStart, int width, int height, const HINSTANCE& appHandle, int windowShowParams)
		{
			window.initWindow(title, xStart, yStart, width, height, appHandle, windowShowParams);
		}



		/// <param name="fov">in degrees</param>
		void initCamera(const float3& position, const float3& direction, const float3& cameraUp, float fov, float aspect, float zNear, float zFar)
		{
			camera.setView(position, direction, cameraUp);
			camera.setPerspective(DirectX::XMConvertToRadians(fov), aspect, zNear, zFar);
			camera.initBuffer(CONSTANT_REGISTER_0, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE); // may be moved in application
		}

		void addInstancedModel(const std::shared_ptr<Model>& model, size_t meshIndex, const std::shared_ptr<OpaqueInstances::Material>& material, const std::vector<OpaqueInstances::Instance>& instances)
		{
			MeshSystem::getInstance().addOpaqueInstances(model, meshIndex, material, instances);
		}

		void setVertexShader(const std::wstring& fileName)
		{
			MeshSystem::getInstance().setVertexShader(fileName);
		}

		void setPixelShader(const std::wstring& fileName)
		{
			MeshSystem::getInstance().setPixelShader(fileName);
		}

		void moveCamera(const float3& offset)
		{
			camera.addRelativeOffset(offset);
		}

		void rotateCamera(const Angles& angles)
		{
			camera.setWorldAngles(angles);
		}

		const float3& getCameraForward() const
		{
			return camera.forward();
		}

		const float3& getCameraRight() const
		{
			return camera.right();
		}

		const float3& getCameraUp() const
		{
			return camera.up();
		}
			
		const float3& getCameraPosition() const
		{
			return camera.position();
		}

		void render()
		{
			checkAspectRatio();
			renderer.render(window, camera);
		}


	private:
		Renderer renderer;
		Window window;
		Camera camera;

		void checkAspectRatio()
		{
			float windowRatio{ window.GetAspectRatio() };
			if (windowRatio != camera.getAspectRatio())
			{
				camera.changeAspectRatio(windowRatio);
			}
		}
	};
}


